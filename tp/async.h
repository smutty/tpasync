#pragma once

#include "internals/thread_pool.h"
#include <future>

namespace tp {

void init_pool(size_t num_workers = 0)
{
    internals::thread_pool::instance(num_workers);
}

template<typename Func, typename... Args>
auto async(Func&& func, Args&&... args)
{
    using result_type = decltype(func(args...));

    auto& pool = tp::internals::thread_pool::instance();

    std::packaged_task<result_type()> task{
        std::bind(std::forward<Func>(func), std::forward<Args>(args)...)};

    auto result = task.get_future();

    pool.submit_task(std::move(task));

    return result;
}

} // namespace tp