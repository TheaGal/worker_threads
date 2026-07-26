#pragma once

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
using job_batch_t = std::vector<job_t>;
using job_batch_key_t = uint64_t;

/// Submits a job as a single-job batch.
job_batch_key_t submit_job(job_t&& job);

/// Submits a job batch for execution.
job_batch_key_t submit_job_batch(job_batch_t&& batch);

/// Checks whether the job batch has finished.
/// @note this is an atomic check, so this check is required for memory visibility with other
///       variables involved with the job batch.
bool has_job_batch_finished(job_batch_key_t key);

} // namespace worker_threads
} // namespace THEA
