#pragma once

#include <chrono>
#include <coroutine>

#include <coro/pool/event_pool.hpp>

namespace coro {

struct AsyncSleep {
    std::chrono::milliseconds duration;

    bool await_ready() const noexcept;
    void await_suspend(std::coroutine_handle<> h) noexcept;
    void await_resume() noexcept;
};

} // namespace coro