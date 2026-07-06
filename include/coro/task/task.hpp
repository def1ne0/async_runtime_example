#pragma once

#include <coroutine>
#include <optional>

#include "../pool/event_pool.hpp"

namespace coro {

template <typename T>
class Task {
public:
    struct promise_type;
    using coro_handle_t = std::coroutine_handle<promise_type>;

    struct promise_type {
        std::optional<T> result_{};
        std::exception_ptr exception_;
        std::coroutine_handle<> continuation_{nullptr};

        struct final_awaiter;

        auto get_return_object() {
            return coro_handle_t::from_promise(*this);
        }

        auto initial_suspend() noexcept {
            return std::suspend_always{};
        }

        auto final_suspend() noexcept {
            return final_awaiter{};
        }

        template <typename U>
        void return_value(U&& value) { result_.emplace(std::forward<U>(value)); }

        void unhandled_exception() { exception_ = std::current_exception(); }

        struct final_awaiter {
            bool await_ready() noexcept { return false; }

            std::coroutine_handle<> await_suspend(coro_handle_t handle) noexcept {
                if (handle.promise().continuation_) {
                    return handle.promise().continuation_;
                }
                return std::noop_coroutine();
            }

            void await_resume() noexcept {}
        };
    };

    bool await_ready() const noexcept {
        return false;
    }

    void await_suspend(coro_handle_t continuation) noexcept {
        handle_.promise().continuation_ = continuation;
        g_loop.schedule(handle_);
    }

    T await_resume() {
        if (handle_.promise().exception_) {
            std::rethrow_exception(handle_.promise().exception_);
        }

        return std::move(handle_.promise().result_.value());
    }

    Task(coro_handle_t h) : handle_(h) {}
    ~Task() { if (handle_) handle_.destroy(); }

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    Task(Task&& other) noexcept : handle_(std::exchange(other.handle_, nullptr)) {}
    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = std::exchange(other.handle_, nullptr);
        }
        return *this;
    }

    coro_handle_t Handle() { return handle_; }

private:
    coro_handle_t handle_;
};

struct AsyncSleep {
    std::chrono::milliseconds duration;

    bool await_ready() const noexcept { return duration.count() <= 0; }

    void await_suspend(std::coroutine_handle<> h) noexcept {
        const auto wake_time = EventPool::clock_t::now() + duration;
        g_loop.wait_until(h, wake_time);
    }

    void await_resume() noexcept {}
};

} // namespace coro


