#include <bits/stdc++.h>
#include <chrono>
#include <mutex>
#include <thread>

#include "RingBuffer.hpp"

using namespace std;
using namespace chrono;

/// MESSAGE STRUCTURE
struct Message {
    int64_t type;
    string payload;
    uint64_t timestamp; // HIGH PRECISION, LARGE RANGE, UNSIGNED
};

// TIMESTAMP
uint64_t _timestamp() {
    return duration_cast<nanoseconds>(
               high_resolution_clock::now().time_since_epoch())
        .count();
}

class MessageBroker {
private:
    // MUTEX
    mutex mtx;

    // SUBSCRIBER QUEUE: TOPIC -> (CONSUMER ID -> FIFO RingBuffer)
    unordered_map<int64_t, unordered_map<int64_t, RingBuffer<Message, 1024>>> subscriberQueue;

public:
    //SUBSCRIBE
    void subscribe(int64_t topic, int64_t consumer_id) {
        lock_guard<mutex> lock(mtx);
        auto &map = subscriberQueue[topic];
        // try_emplace avoids ambiguous pair constructor
        map.try_emplace(consumer_id); // default-construct RingBuffer
    }

    //PUBLISH
    void publish(int64_t type, int64_t topic, const string &data) {
        lock_guard<mutex> lock(mtx);
        Message msg{type, data, _timestamp()};
        auto it = subscriberQueue.find(topic);
        if (it != subscriberQueue.end()) {
            for (auto &kv : it->second) {
                kv.second.push(msg);
            }
        }
        cout << "topic " << topic
             << " msg=" << msg.payload
             << " type=" << msg.type
             << " timestamp=" << msg.timestamp << endl;
    }

    //CONSUME
    bool consume(int64_t topic, int64_t consumer_id, Message &out) {
        lock_guard<mutex> lock(mtx);
        auto t_it = subscriberQueue.find(topic);
        if (t_it != subscriberQueue.end()) {
            auto &map = t_it->second;
            auto c_it = map.find(consumer_id);
            if (c_it != map.end()) {
                return c_it->second.pop(out);
            }
        }
        return false;
    }
};

int main() {
    MessageBroker broker;

    broker.subscribe(1, 1001);
    broker.subscribe(1, 1002);

    auto producer = [&broker]() {
        for (int i = 0; i < 10; i++) {
            broker.publish(1, 1, "msg_" + to_string(i));
            this_thread::sleep_for(milliseconds(100));
        }
    };

    auto consumer = [&broker](int64_t id) {
        for (int i = 0; i < 10; i++) {
            Message msg;
            if (broker.consume(1, id, msg)) {
                cout << "Consumer " << id << " got: " << msg.payload
                     << " at " << msg.timestamp << endl;
            } else {
                cout << "Consumer " << id << " has no message" << endl;
            }
            this_thread::sleep_for(milliseconds(150));
        }
    };

    thread t1(producer);
    thread t2(consumer, 1001);
    thread t3(consumer, 1002);

    t1.join();
    t2.join();
    t3.join();

    return 0;
}
