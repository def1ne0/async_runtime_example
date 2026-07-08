#pragma once

#include <coroutine>

namespace coro {

template <typename T>
concept pool_type = requires(T pool, std::coroutine_handle<> h) {
    { pool.Schedule(h) } -> std::same_as<void>;
};

} // namespace coro