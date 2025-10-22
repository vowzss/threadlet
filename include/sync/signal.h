#pragma once

namespace threadlet::sync {
    struct signal {
        bool cancelled = false;
        bool paused = false;
    };
}