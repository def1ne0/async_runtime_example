/* ==============================================
 * This file is under MIT License.
 * For details, see LICENSE.md
 *
 * Copyright (c) 2026 Arseniy Finevich
 * ==============================================
 */

module;

#include <chrono>    // for std::chrono::milliseconds
#include <coroutine> // for coroutine magic

export module coro.async_sleep;

namespace coro {

export struct AsyncSleep final {
    std::chrono::milliseconds duration;

    bool await_ready() const noexcept;
    void await_suspend(std::coroutine_handle<> h) noexcept;
    void await_resume() noexcept;
};

} // namespace coro