#pragma once

#include <memory>

#include "threadlet/priority.h"

namespace threadlet::types {
    class runnable {
      protected:
        priority level_;
        uint8_t urgency_;

      public:
        runnable(priority level, uint8_t urgency) : level_(level), urgency_(urgency) {}
        virtual ~runnable() = default;

        virtual void execute() = 0;

        struct comparator {
            bool operator()(const std::shared_ptr<types::runnable>& a, const std::shared_ptr<types::runnable>& b) const { return *a < *b; }
        };

        bool operator<(const runnable& other) const
        {
            if (level_ != other.level_)
                return level_ < other.level_;

            return urgency_ < other.urgency_;
        }
    };
}