#pragma once

#include <concepts>

#include <coro/pool/pool_concept.hpp>

namespace coro {

template <typename Task, typename Pool>
concept task_type = requires(Task task, Pool pool) {
    requires pool_type<Pool>;
    { task.Attach(pool) } -> std::same_as<void>;
};

} // namespace coro
