#include <mutex>
#include <functional>
#include <queue>
#include <condition_variable>

#pragma once

using Job = std::function<void()>;

struct JobQueue {
    void push(Job job) {
        std::unique_lock<std::mutex> unique_lock(job_queue_mutex);
        job_queue.push(job);
        cv_job_queue_is_not_empty.notify_one();
    }

    Job pop() {
        std::unique_lock<std::mutex> unique_lock(job_queue_mutex);
        cv_job_queue_is_not_empty.wait(unique_lock, [this](){
            return !job_queue.empty();
        });
        const auto ret = job_queue.front();
        job_queue.pop();
        return ret;
    }
private:
    std::condition_variable cv_job_queue_is_not_empty;
    std::mutex job_queue_mutex;
    std::queue<Job> job_queue;
};