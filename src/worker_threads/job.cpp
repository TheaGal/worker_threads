#include "worker_threads/job.h"

#include <atomic>
#include <cassert>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <thread>


namespace THEA
{
namespace
{

using namespace worker_threads;

struct Job_batch
{
    std::vector<job_t> job_list;
    std::atomic_uint32_t next_reserve_idx{ 0 };
    std::atomic_uint32_t num_jobs_complete{ 0 };
    std::atomic_bool batch_complete{ false };
};

class Job_queue
{
public:
    static constexpr uint32_t k_queue_size{ 1024 };  // must be power of 2.

    static uint32_t key_to_index(job_batch_key_t key)
    {
        return (key & (Job_queue::k_queue_size - 1));
    }

    Job_batch& at(uint32_t idx)
    {
        return m_queue.at(idx);
    }

    std::array<Job_batch, k_queue_size> m_queue;
    std::atomic<job_batch_key_t> m_next_emplace_key{ 0 };
    std::atomic<job_batch_key_t> m_next_read_key{ 0 };
};

void worker_thread_run_fn(Job_queue& job_queue, std::atomic_bool& exit_flag)
{
    while (!exit_flag.load(std::memory_order_relaxed))
    {
        job_batch_key_t key{ job_queue.m_next_read_key.load() };
        auto& job_batch = job_queue.at(Job_queue::key_to_index(key));

        // @NOTE: no need to check whether the batch is complete here. Just reserve a job and try to
        //        run it. Only after a job is finished do you see if the batch should be marked as
        //        finished.

        // Reserve work!!
        uint32_t reserve_idx{ job_batch.next_reserve_idx++ };
        uint32_t num_jobs = job_batch.job_list.size();
        if (reserve_idx < num_jobs)
        {
            // Do work on current job!
            job_batch.job_list.at(reserve_idx)();

            // Mark the batch as complete if it's done here.
            uint32_t num_jobs_complete{ ++job_batch.num_jobs_complete };
            if (num_jobs_complete == num_jobs)
            {
                job_batch.batch_complete.store(true);
                job_batch.batch_complete.notify_all();
            }
            else if (num_jobs_complete > num_jobs)
            {
                assert(false);
                throw std::runtime_error(
                    "You shouldn't have made it here. There is some concurrency error if so.");
            }
        }
        else
        {
            // This is invalid number! Move to next batch.
            job_batch_key_t next_key{ key + 1 };
            (void)job_queue.m_next_read_key.compare_exchange_strong(key, next_key);
        }
    }
}

class Worker_thread_group
{
public:
    Worker_thread_group(uint32_t num_threads)
    {
        m_threads.reserve(num_threads);
        for (uint32_t i = 0; i < num_threads; i++)
            m_threads.emplace_back(worker_thread_run_fn,
                                   std::ref(m_queue),
                                   std::ref(m_exit_threads_flag));
    }

    Worker_thread_group(Worker_thread_group const&)             = delete;
    Worker_thread_group& operator=(Worker_thread_group const&)  = delete;
    Worker_thread_group(Worker_thread_group const&&)            = delete;
    Worker_thread_group& operator=(Worker_thread_group const&&) = delete;

    ~Worker_thread_group()
    {
        m_exit_threads_flag.store(true);
        for (auto& t : m_threads)
            t.join();
    }

    uint32_t get_num_threads() const
    {
        return m_threads.size();
    }

    job_batch_key_t job_batch_emplace_back(Job_batch&& batch)
    {
        job_batch_key_t key{ m_queue.m_next_emplace_key++ };
        uint32_t idx{ Job_queue::key_to_index(key) };

        // Perform "move".
        auto& dest_job_batch{ m_queue.at(idx) };
        dest_job_batch.job_list = std::move(batch.job_list);
        dest_job_batch.next_reserve_idx.store(batch.next_reserve_idx.load());
        dest_job_batch.num_jobs_complete.store(batch.num_jobs_complete.load());
        dest_job_batch.batch_complete.store(batch.batch_complete.load());  // Must be last.

        return key;
    }

    Job_batch const& get_job_batch_ref(job_batch_key_t key) const
    {
        uint32_t idx{ key & (Job_queue::k_queue_size - 1) };
        return const_cast<Job_queue&>(m_queue).at(idx);
    }

private:
    std::vector<std::thread> m_threads;
    std::atomic_bool m_exit_threads_flag{ false };
    Job_queue m_queue;
};

std::unique_ptr<Worker_thread_group> g_thread_group{ nullptr };

} // namespace


uint32_t worker_threads::calc_ideal_num_threads()
{
    uint32_t ideal_threads{ std::thread::hardware_concurrency() };
    if (ideal_threads > 1)
        ideal_threads--;
    return ideal_threads;
}

void worker_threads::init_worker_threads(uint32_t num_threads)
{
    g_thread_group = std::make_unique<Worker_thread_group>(num_threads);
}

auto worker_threads::submit_job(job_t&& job) -> job_batch_key_t
{
    return submit_job_batch({ job });
}

auto worker_threads::submit_job_batch(std::vector<job_t>&& batch) -> job_batch_key_t
{
    return g_thread_group->job_batch_emplace_back(Job_batch{ .job_list = std::move(batch) });
}

bool worker_threads::has_job_batch_finished(job_batch_key_t key)
{
    return g_thread_group->get_job_batch_ref(key).batch_complete.load();
}

void worker_threads::wait_until_job_batch_finished(job_batch_key_t key)
{
    // Wait until `batch_complete` is not false.
    g_thread_group->get_job_batch_ref(key).batch_complete.wait(false);
}

} // namespace THEA
