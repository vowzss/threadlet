#pragma once

#include <algorithm>
#include <array>
#include <random>

#include <uuid.h>

namespace threadlet::utils {
    inline auto uuid() -> uuids::uuid
    {
        thread_local std::mt19937 generator = [] {
            auto seed = std::array<std::uint32_t, std::mt19937::state_size>{};
            std::random_device rd;

            std::generate(seed.begin(), seed.end(), std::ref(rd));
            std::seed_seq seq(seed.begin(), seed.end());

            return std::mt19937(seq);
        }();

        thread_local uuids::uuid_random_generator gen(generator);
        return gen();
    }
}