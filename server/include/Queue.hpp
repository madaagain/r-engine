

#ifndef QUEUE_HPP_
#define QUEUE_HPP_

#include <deque>
#include <mutex>

namespace net
{
    template <typename T>
    class Queue {
    public:
        Queue() = default;
        Queue(const Queue<T>&) = delete;
        virtual ~Queue() { clear(); }

    public:
        const T& front()
        {
            std::scoped_lock lock(queue_mutex);
            return message_queue.front();
        }

        const T& back()
        {
            std::scoped_lock lock(queue_mutex);
            return message_queue.back();
        }

        void pushBack(const T& item)
        {
            std::scoped_lock lock(queue_mutex);
            message_queue.emplace_back(std::move(item));
        }

        void pushFront(const T& item)
        {
            std::scoped_lock lock(queue_mutex);
            message_queue.emplace_front(std::move(item));
        }

        bool empty()
        {
            std::scoped_lock lock(queue_mutex);
            return message_queue.empty();
        }

        size_t count()
        {
            std::scoped_lock lock(queue_mutex);
            return message_queue.size();
        }

        void clear()
        {
            std::scoped_lock lock(queue_mutex);
            message_queue.clear();
        }

        T popFront()
        {
            std::scoped_lock lock(queue_mutex);
            auto t = std::move(message_queue.front());
            message_queue.pop_front();
            return t;
        }

        T popBack()
        {
            std::scoped_lock lock(queue_mutex);
            auto t = std::move(message_queue.back());
            message_queue.pop_back();
            return t;
        }

    protected:
        std::mutex queue_mutex;
        std::deque<T> message_queue;
    };
}

#endif /* !QUEUE_HPP_ */
