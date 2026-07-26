#include "worker_threads_public.h"

#include <cstdint>
#include <iostream>


void some_job()
{
    TIMESLICER_SCOPED(ONES, 0);
    std::cout << __func__ << "()\n";
}

void some_job2()
{
    TIMESLICER_SCOPED(TWOS, 0);
    std::cout << __func__ << "()\n";
}

void some_job3()
{
    TIMESLICER_SCOPED(TWOS, 1);
    std::cout << __func__ << "()\n";
}

void some_job4()
{
    TIMESLICER_SCOPED(EIGHTS, 3);
    std::cout << __func__ << "()\n";
}


int32_t main()
{
    THEA::worker_threads::Worker_thread_group worker_thread_group;
    THEA::worker_threads::set_submission_thread_group(worker_thread_group);

    for (size_t i = 0; i < 500; i++)
    {
        TIMESLICER_TICK;
        std::cout << "--------------- Tick " << i << " ---------------\n";

        some_func();
        some_func2();
        some_func3();
        some_func4();
    }

    std::cout << "-=-=-=-=-=-=-=- FINISH -=-=-=-=-=-=-=-\n";

    TIMESLICER_PRINT_PERFORMANCE_REPORT;
    TIMESLICER_PRINT_TIMESLICE_PATTERN_REPORT;

    return 0;
}
