#ifndef MESSAGE_BROKER_HPP
#define MESSAGE_BROKER_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <cstdint>
#include <chrono>

#include "RingBuffer.hpp"

struct Message {
    int64_t type;
    char payload[256];
    uint64_t timestamp;

    Message() : type(0), payload{0}, timestamp(0) {}
};

inline uint64_t getCurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               std::chrono::high_resolution_clock::now().time_since_epoch())
        .count();
}

struct TopicData {
    std::mutex topic_mtx;
    std::unordered_map<int64_t, RingBuffer<Message, 1024>> subscribers;
};

class MessageBroker {
private:
    std::mutex map_mtx;
    std::unordered_map<int64_t, std::shared_ptr<TopicData>> topics;

public:
    void subscribe(int64_t topic_id, int64_t consumer_id);
    void publish(int64_t type, int64_t topic_id, const std::string& data);
    bool consume(int64_t topic_id, int64_t consumer_id, Message& out);
    bool consume_batch(int64_t topic_id, int64_t consumer_id, std::vector<Message>& out, size_t max_items);
};

#endif // MESSAGE_BROKER_HPP