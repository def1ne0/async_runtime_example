/* ==============================================
 * This file is under MIT License.
 * For details, see LICENSE.md
 *
 * Copyright (c) 2026 Arseniy Finevich
 * ==============================================
 */

module;

#include <coroutine> // for std::coroutine_handle

export module coro.pool_concept;

namespace coro {

export
template <typename T>
concept pool_type = requires(T pool, std::coroutine_handle<> h) {
    { pool.add_coro_handle(h) } -> std::same_as<void>;
};

} // namespace coro