#pragma once

#include <future>

#include "threadlet/pool.h"
#include "threadlet/sync/signal.h"

namespace threadlet {
    template <typename F> class task final : public types::runnable {
        using R = std::invoke_result_t<F>;

      public:
        static inline std::future<R> run(F&& job, priority level = priority::MEDIUM, uint8_t urgency = 1, std::shared_ptr<sync::signal> signal = nullptr)
        {
            auto self = std::make_shared<task<F>>(std::forward<F>(job), level, urgency, signal);
            auto future = self->job_.get_future();

            pool::instance().enqueue(self);

            return future;
        }

        void execute() override { job_(); }

      private:
        std::packaged_task<R()> job_;

        std::shared_ptr<sync::signal> signal_;
        // TODO: impl deadline (std::chrono::steady_clock::time_point)

        task(F&& job, priority level, uint8_t urgency, std::shared_ptr<sync::signal> signal)
            : runnable(level, urgency), job_(std::forward<F>(job)), signal_(std::move(signal))
        {
        }

        task(const task&) = delete;
        task& operator=(const task&) = delete;

        ~task() = default;
    };
}
