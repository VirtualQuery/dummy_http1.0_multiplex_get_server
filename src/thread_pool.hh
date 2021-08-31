#include "job_queue.hh"

#include <thread>
#include <vector>

#pragma once

struct ThreadPool {
    ThreadPool() {
        for(size_t i = 0; i < std::thread::hardware_concurrency(); ++i) {
            thread_pool.push_back(
                std::thread(
                    std::bind(
                        &ThreadPool::loop,
                        this
                    )
                )
            );
        }
    }

    void loop() {
        while(true) {
            const auto job = job_queue.pop();
            job();
        }
    }

    void add_job(JobQueue::Job job) {
        job_queue.push(job);
    }

private:
    JobQueue job_queue;
    std::vector<std::thread> thread_pool;
};
