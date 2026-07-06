#pragma once

#include <coroutine>
#include <exception>
#include <utility>
#include <print>

namespace coro {

template <typename T>
class Generator {
public:
    struct promise_type;
    using coro_handle_t = std::coroutine_handle<promise_type>;

    struct promise_type {
        const T* value_{nullptr};

        auto get_return_object() { return coro_handle_t::from_promise(*this); }
        auto initial_suspend() noexcept { return std::suspend_always{}; }
        auto final_suspend() noexcept { return std::suspend_always{}; }
        auto yield_value(const T& value) {
            value_ = std::addressof(value);
            std::println("Got value: ", value);
            return std::suspend_always{};
        }
        void return_void() noexcept {}
        void unhandled_exception() { std::terminate(); }
    };

private:
    coro_handle_t handle_;

public:
    Generator(coro_handle_t handle) : handle_(handle) {}
    // Delete copy ctor and copy operator=.
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    // Move ctor and move operator=
    Generator(Generator&& rhs) noexcept : handle_(std::exchange(rhs.handle_, std::coroutine_handle<>{})) {}
    ~Generator() { if (handle_) handle_.destroy(); }
    Generator& operator=(Generator&& rhs) noexcept {
        if (this != &rhs) {
            if (handle_)
                handle_.destroy();
            handle_ = rhs.handle_;
            rhs.handle_ = std::coroutine_handle<>{};
        }

        return *this;
    }

    bool move_next() {
        if (!handle_.done()) {
            handle_.resume();
        }

        return !handle_.done();
    }

    T current_value() {
        return *handle_.promise().value_;
    }
};

} // namespace coro
