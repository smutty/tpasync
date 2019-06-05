#pragma once

#include <memory>
#include <mutex>

namespace tp { namespace internals {

template<typename T>
class threadsafe_queue {
    struct node {
        std::unique_ptr<node> next;
        std::shared_ptr<T> data;
    };

    std::unique_ptr<node> head;
    mutable std::mutex head_mtx;

    node* tail{nullptr};
    mutable std::mutex tail_mtx;

public:
    threadsafe_queue()
        : head{std::make_unique<node>()}
        , tail{head.get()}
    {}

    threadsafe_queue(const threadsafe_queue&) = delete;
    threadsafe_queue& operator=(const threadsafe_queue&) = delete;

    void push(T new_value)
    {
        auto new_node = std::make_unique<node>();
        auto new_data = std::make_shared<T>(std::move(new_value));
        {
            std::lock_guard<std::mutex> tail_lock{tail_mtx};
            tail->next = std::move(new_node);
            tail->data = std::move(new_data);
            tail = tail->next.get();
        }
    }

    bool try_pop(T& data)
    {
        std::lock_guard<std::mutex> head_lock{head_mtx};
        {
            std::lock_guard<std::mutex> tail_lock{tail_mtx};
            if (head.get() == tail) {
                return false;
            }
        }
        data = std::move(*head->data);
        auto old_head = std::move(head);
        head = std::move(old_head->next);
        return true;
    }
};

}} // namespace tp::internals
