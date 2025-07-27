#ifndef RINGBUFFER_HPP
#define RINGBUFFER_HPP

#include <atomic>
#include <cstddef>

template<typename T, size_t Size>
class RingBuffer{
    private:
    std::atomic<size_t> head;
    std::atomic<size_t> tail;
    T buffer[Size];

    public:
    RingBuffer(){
        head.store(0);  // READ (DEQUEUE)
        tail.store(0);  // WRITE (ENQUE)   
    }

    bool push(T item){  
        size_t t= tail.load(std::memory_order_relaxed);
        size_t h= head.load(std::memory_order_acquire);

        if(((t+1)% Size)==h){
            return false; // FULL
        }

        buffer[t]= item;
        tail.store((t+1)%Size, std::memory_order_release);
        return true;
    }

    bool pop(T &item){
        size_t h= head.load(std::memory_order_relaxed);
        size_t t= tail.load(std::memory_order_acquire);

        if(h==t) return false; // EMPTY
        item= buffer[h];
        head.store((h+1)%Size, std::memory_order_release);
        return true;
    }

    bool empty(){
        return head.load(std::memory_order_relaxed) == tail.load(std::memory_order_relaxed);
    }

    bool full(){
        return ((tail.load(std::memory_order_relaxed)+1)%Size)==head.load(std::memory_order_relaxed);
    }

    size_t capacity(){
        return Size;
    }
};

#endif // RINGBUFFER_HPP