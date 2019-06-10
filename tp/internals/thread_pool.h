#pragma once

#include <future>
#include <memory>
#include <thread>
#include <vector>
#include "threadsafe_queue.h"

namespace tp { namespace internals {

class thread_pool;

class task_wrapper {
    friend class thread_pool;

    struct task_base {
        virtual void call() = 0;
        virtual ~task_base() = default;
    };

    template<typename Task>
    struct task_impl : task_base {
        task_impl(Task&& t) noexcept
            : task{std::move(t)}
        {}

        void call() override { task(); }

    private:
        Task task;
    };

    std::unique_ptr<task_base> impl;

public:
    task_wrapper() = default;

    template<typename Func>
    task_wrapper(Func&& func)
        : impl{std::make_unique<task_impl<Func>>(std::move(func))}
    {}

    void operator()() const { impl->call(); }

    // move only
    task_wrapper(task_wrapper&&) = default;
    task_wrapper& operator=(task_wrapper&&) = default;

    task_wrapper(task_wrapper&) = delete;
    task_wrapper(const task_wrapper&) = delete;
    task_wrapper& operator=(const task_wrapper&) = delete;
};


class thread_pool {
public:
    static thread_pool& instance(size_t num_workers = 0)
    {
        static thread_pool tp{num_workers};
        return tp;
    }

    ~thread_pool()
    {
        done = true;
        for (auto& worker : workers) {
            worker.join();
        }
    }

    void submit_task(task_wrapper task)
    {
        if (worker_thread()) {
            // FIXME - all recursive submits will be processed within the same worker
            task();
        } else {
            work_queue.push(std::move(task));
        }
    }

    void try_consume_task()
    {
        task_wrapper task;
        if (work_queue.try_pop(task)) {
            task();
        } else {
            std::this_thread::yield();
        }
    }

private:
    thread_pool(size_t num_workers_)
        : num_workers{num_workers_}
    {
        try {
            if (!num_workers) {
                num_workers = std::thread::hardware_concurrency();
            }
            for (size_t i = 0; i < num_workers; ++i) {
                workers.emplace_back([this] {
                    worker_thread() = true;
                    while (!done) {
                        try_consume_task();
                    }
                });
            }
        }
        catch (...) {
            done = true;
            throw;
        }
    }

    bool& worker_thread()
    {
        static thread_local bool wt = false;
        return wt;
    }

    threadsafe_queue<task_wrapper> work_queue;
    std::vector<std::thread> workers;
    size_t num_workers{0};
    std::atomic<bool> done{false};
};

}} // namespace tp::internals