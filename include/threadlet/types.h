#pragma once

#include <memory>

#include "threadlet/task.h"

namespace threadlet {
    using runnable = std::shared_ptr<task<void>>;

    struct comparator {
        inline bool operator()(const runnable& a, const runnable& b) const
        {
            if (a->priority() != b->priority()) {
                return a->priority() > b->priority();
            }

            return a->urgency() < b->urgency();
        }
    }
}