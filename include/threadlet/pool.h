#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "threadlet/types/runnable.h"

namespace threadlet {

    class pool {

      public:
        static pool& instance()
        {
            static pool instance;
            return instance;
        }

      private:
        template <typename F> friend class task_impl;

        std::vector<std::thread> workers_;
        std::priority_queue<std::shared_ptr<types::runnable>, std::vector<std::shared_ptr<types::runnable>>, types::runnable::comparator> jobs_;

        std::mutex mutex_;
        std::condition_variable cv_;
        bool shutdown_ = false;

        explicit pool(size_t n = std::thread::hardware_concurrency())
        {
            for (size_t i = 0; i < n; ++i) {
                workers_.emplace_back([this] { process(); });
            }
        }

        ~pool()
        {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                shutdown_ = true;
            }

            cv_.notify_all();

            for (auto& worker : workers_) {
                worker.join();
            }
        }

        void enqueue(std::shared_ptr<types::runnable> job)
        {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                jobs_.emplace(std::move(job));
            }

            cv_.notify_one();
        }

        void process()
        {
            while (!shutdown_) {
                std::shared_ptr<types::runnable> job;

                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    cv_.wait(lock, [&] { return shutdown_ || !jobs_.empty(); });

                    if (shutdown_ && jobs_.empty())
                        return;

                    job = jobs_.top();
                    jobs_.pop();
                }

                job->execute();
            }
        }
    };
}