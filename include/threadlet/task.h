#pragma once

#include <future>

#include "threadlet/pool.h"
#include "threadlet/sync/signal.h"

namespace threadlet {
    template <typename F> class task_impl final : public types::runnable {
        using R = std::invoke_result_t<F>;

        std::packaged_task<R()> job_;
        std::shared_ptr<sync::signal> signal_;

        // TODO: impl deadline (std::chrono::steady_clock::time_point)

      public:
        task_impl(F&& job, priority level, uint8_t urgency, std::shared_ptr<sync::signal> signal)
            : runnable(level, urgency), job_(std::forward<F>(job)), signal_(std::move(signal))
        {
        }

        task_impl(const task_impl&) = delete;
        task_impl& operator=(const task_impl&) = delete;

        ~task_impl() = default;

        static inline auto perform(F&& job, priority level = priority::MEDIUM, uint8_t urgency = 1, std::shared_ptr<sync::signal> signal = nullptr)
        {
            auto self = std::make_shared<task_impl<F>>(std::forward<F>(job), level, urgency, signal);
            auto future = self->job_.get_future();

            pool::instance().enqueue(self);
            return future;
        }

        void execute() override { job_(); }
    };

    class task {
      public:
        template <typename F, typename... Args> static auto run(F&& f, Args&&... args)
        {
            return task_impl<std::decay_t<F>>::perform(std::forward<F>(f), std::forward<Args>(args)...);
        }
    };
}
