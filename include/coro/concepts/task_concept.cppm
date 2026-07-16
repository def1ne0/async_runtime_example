/* ==============================================
 * This file is under MIT License.
 * For details, see LICENSE.md
 *
 * Copyright (c) 2026 Arseniy Finevich
 * ==============================================
 */

module;

#include <concepts> // for std::same_as
#include <utility> // for std::move

export module coro.task_concept;

import coro.pool_concept;

namespace coro {

export
template <typename Task, typename Pool>
concept task_type = requires(Task task, Pool pool) {
    requires pool_type<Pool>;
    { std::move(task).attach(pool) } -> std::same_as<void>;
};

} // namespace coro
