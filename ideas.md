# Worker thread todo/ideas

- [ ] Create jobs in the form of `std::function`.
- [ ] Create job batches.
- [ ] Submit job batches, get batch key
- [ ] Check whether job is finished.
    - This is an atomic check so anything that got updated is guaranteed to be updated on the current thread as long as the job is confirmed that it is finished.
- [ ] Initialize worker threads.
- [ ] Debugging
    - [ ] Profile the time that the worker threads hand back the work takes.
        - Mmmm this can actually just be done with integration with the tracy profiler.
        - (LATER) provide hooks to make this work with the tracy profiler, like callback functions.

> I'd like to think that this is meant to be a load-balancing tool at the end of the day.
