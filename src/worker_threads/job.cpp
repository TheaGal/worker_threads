#include "worker_threads/job.h"

#include "worker_threads/worker_thread_group.h"


namespace THEA
{
namespace
{

worker_threads::Worker_thread_group* g_thread_group{ nullptr };

worker_threads::Job_queue g_job_queue;

} // namespace


using worker_threads::job_batch_key_t;

void worker_threads::set_submission_thread_group(Worker_thread_group& thread_group)
{
    g_thread_group = &thread_group;
}

job_batch_key_t worker_threads::submit_job(job_t&& job)
{
    return submit_job_batch(Job_batch{ .job_list = { job } });
}

job_batch_key_t worker_threads::submit_job_batch(Job_batch&& batch)
{

}

bool worker_threads::has_job_batch_finished(job_batch_key_t key)
{

}

void worker_threads::wait_until_job_batch_finished(job_batch_key_t key)
{

}

} // namespace THEA
