#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <vector>


namespace THEA
{
namespace worker_threads
{

// Types.
using job_t = std::function<void()>;
struct Job_batch
{
    std::vector<job_t> job_list;
    std::atomic_uint32_t next_reserve_idx{ 0 };
    std::atomic_uint32_t num_jobs_complete{ 0 };
    std::atomic_bool batch_complete{ false };
};
using job_batch_key_t = uint32_t;

/// Helper func for getting ideal number of threads.
uint32_t calc_ideal_num_threads();

/// Initializes worker threads.
void init_worker_threads(uint32_t num_threads = calc_ideal_num_threads());

/// Submits a job as a single-job batch.
job_batch_key_t submit_job(job_t&& job);

/// Submits a job batch for execution.
job_batch_key_t submit_job_batch(Job_batch&& batch);

/// Checks whether the job batch has finished.
/// @note this is an atomic check, so this check is required for memory visibility with other
///       variables involved with the job batch.
bool has_job_batch_finished(job_batch_key_t key);

/// Waits/blocks until the job batch has finished.
void wait_until_job_batch_finished(job_batch_key_t key);

} // namespace worker_threads
} // namespace THEA
