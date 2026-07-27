#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <vector>


namespace THEA
{
namespace worker_threads
{

// Forward decl.
class Worker_thread_group;

/// Configures jobs to submit to this worker thread group.
void set_submission_thread_group(Worker_thread_group& thread_group);

// Types.
using job_t = std::function<void()>;

struct Job_batch
{
    std::vector<job_t> job_list;
    std::atomic_bool batch_complete{ false };
};

using job_batch_key_t = uint64_t;
struct Job_queue
{

};

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
