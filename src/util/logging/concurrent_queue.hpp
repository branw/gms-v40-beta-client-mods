#ifndef MAPLE_01A0B9A20C6848A2A5F86981CF60FBA4_HPP
#define MAPLE_01A0B9A20C6848A2A5F86981CF60FBA4_HPP

#include <condition_variable>
#include <mutex>
#include <queue>

// A generic FIFO queue that allows multi-threaded access
template<typename T>
class ConcurrentQueue {
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cond_;

public:
    ConcurrentQueue() = default;

    ConcurrentQueue(ConcurrentQueue const &) = delete;

    ConcurrentQueue &operator=(ConcurrentQueue const &) = delete;

    template<typename... Args>
    void emplace(Args &&...args) {
        std::lock_guard lock(mutex_);
        queue_.emplace(std::forward<Args>(args)...);
        cond_.notify_one();
    }

    void push(T item) {
        std::lock_guard lock(mutex_);
        queue_.push(item);
        cond_.notify_one();
    }

    bool try_and_pop(T &popped_item) {
        std::lock_guard lock(mutex_);
        if (queue_.empty()) {
            return false;
        }

        popped_item = queue_.front();
        queue_.pop();
        return true;
    }

    void wait_and_pop(T &popped_item) {
        std::unique_lock lock(mutex_);
        cond_.wait(lock, [this] { return !queue_.empty(); });

        popped_item = queue_.front();
        queue_.pop();
    }

    T wait_and_pop() {
        std::unique_lock lock(mutex_);
        cond_.wait(lock, [this] { return !queue_.empty(); });

        auto ref = std::move(queue_.front());
        queue_.pop();
        return ref;
    }

    bool empty() const {
        std::lock_guard lock(mutex_);
        return queue_.empty();
    }

    std::size_t size() const {
        std::lock_guard lock(mutex_);
        return queue_.size();
    }
};

#endif//MAPLE_01A0B9A20C6848A2A5F86981CF60FBA4_HPP
