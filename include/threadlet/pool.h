#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "threadlet/types.h"

namespace threadlet {
    class pool {

      private:
        std::vector<std::thread> workers_;
        std::priority_queue<runnable, std::vector<runnable>, utils::task_comparator> tasks_;

        std::mutex mutex_;
        std::condition_variable cv_;
        bool shutdown_ = false;

      public:
        static pool& global()
        {
            static pool instance;
            return instance;
        }

      public:
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

        template <typename F> runnable enqueue(F&& f, priority priority)
        {
            auto wrapper = [f = std::forward<F>(f)]() mutable { f(); };
            runnable r = std::make_shared<task<void>>(std::move(wrapper), priority, 1);

            {
                std::lock_guard<std::mutex> lock(mutex_);
                tasks_.emplace(r);
            }

            cv_.notify_one();
            return r;
        }

      private:
        void process()
        {
            while (!shutdown_) {
                runnable r;

                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    cv_.wait(lock, [&] { return shutdown_ || !tasks_.empty(); });

                    if (shutdown_ && tasks_.empty())
                        return;

                    r = tasks_.top();
                    tasks_.pop();
                }

                // r.callable();
            }
        }
    };
}