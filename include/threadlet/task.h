#pragma once

#include <functional>
#include <future>

#include "priority.h"
#include "sync/signal.h"
#include "threadlet/pool.h"
#include "utils/uuid.h"

namespace threadlet {
    template <typename R, typename... Args> class task final : public std::enable_shared_from_this<task<R, Args...>> {
        using callable = std::function<R(Args...)>;

      private:
        uuids::uuid id_;
        priority priority_;

        // TODO: impl urgency (uint8_t)
        // TODO: impl deadline (std::chrono::steady_clock::time_point)

        callable callable_;
        std::shared_ptr<sync::signal> signal_;

      private:
        task(callable callable, priority priority, std::shared_ptr<sync::signal> signal)
            : id_(utils::uuid()), priority_(priority), callable_(std::move(callable)), signal_(std::move(signal))
        {
        }

      public:
        static std::future<R> run(callable callable, priority priority = priority::MEDIUM,
                                  std::shared_ptr<sync::signal> signal = std::make_shared<sync::signal>())
        {
            auto t = std::make_shared<task>(callable, priority, signal);
            return t->schedule();
        }

        priority priority() const { return priority_; }
        uint8_t urgency() const { return 1; }

      private:
        std::future<R> schedule()
        {
            auto promise = std::make_shared<std::promise<R>>();
            auto t = this->shared_from_this(); // Keep object alive

            // TODO:
            pool::global().enqueue([t, promise]() { t->perform(promise); });

            return promise->get_future();
        }

        void perform(std::shared_ptr<std::promise<R>> promise)
        {
            try {
                if constexpr (std::is_void_v<R>) {
                    callable_();
                    promise->set_value();
                }
                else {
                    promise->set_value(callable_());
                }
            }
            catch (...) {
                promise->set_exception(std::current_exception());
            }
        }
    };
}
