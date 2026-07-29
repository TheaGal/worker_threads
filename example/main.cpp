#include "worker_threads_public.h"

#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <thread>


int32_t some_job()
{
    std::cout << "started " << __func__ << "()\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "finished " << __func__ << "()\n";
    return 123;
}

struct Four_part
{
    int32_t x{ 0 };
    int32_t y{ 0 };
    int32_t z{ 0 };
    int32_t w{ 0 };
};

void some_job2(int32_t& part, std::string desc)
{
    std::cout << "\tstarted " << __func__ << "(" << desc << ")\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    part++;
    std::cout << "\tfinished " << __func__ << "(" << desc << ")\n";
}


int32_t main()
{
    THEA::worker_threads::init_worker_threads();

    // Submit single job and spin until finished.
    {
        int32_t result = 0;
        assert(result == 0);

        auto jb_key = THEA::worker_threads::submit_job([&result]() { result = some_job(); });

        while (!THEA::worker_threads::has_job_batch_finished(jb_key))
        {
        }
        assert(result == 123);
    }

    // Submit single job and wait.
    {
        int32_t result = 0;
        assert(result == 0);

        auto jb_key = THEA::worker_threads::submit_job([&result]() { result = some_job(); });

        THEA::worker_threads::wait_until_job_batch_finished(jb_key);
        assert(result == 123);
    }

    // Submit many job batches and check on the fly.
    constexpr size_t k_num_many_job_batches{ 1000 };

    std::array<Four_part, k_num_many_job_batches> result_parts;
    std::array<THEA::worker_threads::job_batch_key_t, k_num_many_job_batches> jb_keys;

    for (size_t i = 0; i < k_num_many_job_batches; i++)
    {
        auto& res_prt{ result_parts[i] };
        assert(res_prt.x == 0);
        assert(res_prt.y == 0);
        assert(res_prt.z == 0);
        assert(res_prt.w == 0);

        auto i_str = std::to_string(i);

        switch (i % 4)
        {
        case 0:
            jb_keys[i] = THEA::worker_threads::submit_job(
                [&res_prt, i_str]() { some_job2(res_prt.x, i_str + ",0,x"); });
            break;

        case 1:
            jb_keys[i] = THEA::worker_threads::submit_job_batch(
                { .job_list = {
                      [&res_prt, i_str]() { some_job2(res_prt.x, i_str + ",1,x"); },
                      [&res_prt, i_str]() { some_job2(res_prt.y, i_str + ",1,y"); },
                  } });
            break;

        case 2:
            jb_keys[i] = THEA::worker_threads::submit_job_batch(
                { .job_list = {
                      [&res_prt, i_str]() { some_job2(res_prt.x, i_str + ",2,x"); },
                      [&res_prt, i_str]() { some_job2(res_prt.y, i_str + ",2,y"); },
                      [&res_prt, i_str]() { some_job2(res_prt.z, i_str + ",2,z"); },
                  } });
            break;

        case 3:
            jb_keys[i] = THEA::worker_threads::submit_job_batch(
                { .job_list = {
                      [&res_prt, i_str]() { some_job2(res_prt.x, i_str + ",3,x"); },
                      [&res_prt, i_str]() { some_job2(res_prt.y, i_str + ",3,y"); },
                      [&res_prt, i_str]() { some_job2(res_prt.z, i_str + ",3,z"); },
                      [&res_prt, i_str]() { some_job2(res_prt.w, i_str + ",3,w"); },
                  } });
            break;
        }
    }

    std::array<bool, k_num_many_job_batches> which_batches_finished{};  // zero initialize.
    size_t num_batches_finished{ 0 };

    while (num_batches_finished < k_num_many_job_batches)
    {
        for (size_t i = 0; i < k_num_many_job_batches; i++)
        {
            if (which_batches_finished[i])
            {
                continue;
            }
            else if (THEA::worker_threads::has_job_batch_finished(jb_keys[i]))
            {
                // Check the work.
                auto const& res_prt{ result_parts[i] };
                auto mod4{ i % 4 };
                assert(res_prt.x == 1);
                assert(res_prt.y == (mod4 < 1 ? 0 : 1));
                assert(res_prt.z == (mod4 < 2 ? 0 : 1));
                assert(res_prt.w == (mod4 < 3 ? 0 : 1));

                // Report finished this job.
                which_batches_finished[i] = true;
                num_batches_finished++;
                std::cout << "Finished job " << i << " with correct data. (" << num_batches_finished
                          << "/" << k_num_many_job_batches << ")\n";
            }
        }
    }

    return 0;
}
