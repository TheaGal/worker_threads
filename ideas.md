# Worker thread todo/ideas

- [x] Create jobs in the form of `std::function`.
- [x] Create job batches.
- [x] Submit job batches, get batch key
- [x] Check whether job is finished.
    - This is an atomic check so anything that got updated is guaranteed to be updated on the current thread as long as the job is confirmed that it is finished.
- [x] Initialize worker threads.
- [ ] Debugging
    - [ ] Profile the time that the worker threads hand back the work takes.
        - Mmmm this can actually just be done with integration with the tracy profiler.
        - (LATER) provide hooks to make this work with the tracy profiler, like callback functions.

> I'd like to think that this is meant to be a load-balancing tool at the end of the day.


## Job queue design

- `job.cpp` owns it as a static-mem var.
- Gives reference to the worker-thread-group obj.
- Hides an array of job batches (with `batch_complete` eq true).
- Has `emplace_back()` to move the job batch object.
    - it's thread-safe in that the atomic counter is updated for the write idx.
    - But all it does is assign the atomic and move the rest of the vars.
    - If the array is full, then a warning msg is printed that the presuure is too high and the call blocks until the next available spot is open and 
- Hmmm, im starting to think that maybe the job queue should just be owned by the worker-thread-group. So then it can have all those functions.
    - If that's the case, maybe worker_thread_group.h should be erased and the class should just be in job.cpp