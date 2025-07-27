<<<<<<< HEAD
// For setting CPU affinity on Linux
#ifdef __linux__
#include <pthread.h>
#endif

#include "messagebroker.hpp"

#include <iostream>
#include <thread>
#include <vector>
#include <numeric>
#include <algorithm>
#include <string>

void pinThreadToCore(std::thread& t, int core_id) {
#ifdef __linux__
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    int rc = pthread_setaffinity_np(t.native_handle(), sizeof(cpu_set_t), &cpuset);
    if (rc != 0) {
        std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
    }
#endif
}


const int NUM_MESSAGES = 5000;
const int64_t TOPIC = 1;
const int64_t CONSUMER_ID = 42;

int main() {
    std::cout<<"start"<<std::endl;
    MessageBroker broker;
    broker.subscribe(TOPIC, CONSUMER_ID);
    std::cout<<"debug"<<std::endl;
    std::vector<uint64_t> latencies;
    latencies.reserve(NUM_MESSAGES);
    std::cout<<"debug"<<std::endl;

    std::thread producer([&]() {
        for (int i = 0; i < NUM_MESSAGES; ++i) {
            broker.publish(1, TOPIC, "msg_"); 
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    });
    std::cout<<"debug"<<std::endl;

    std::thread consumer([&]() {
        int received = 0;
        while (received < NUM_MESSAGES) {
            Message msg;
            if (broker.consume(TOPIC, CONSUMER_ID, msg)) {
                uint64_t recv_time = getCurrentTimestamp();
                uint64_t latency = recv_time - msg.timestamp;
                latencies.push_back(latency);
                ++received;
            } else {
                // std::this_thread::yield();
                continue;
            }
        }
    });

    pinThreadToCore(producer, 1);
    pinThreadToCore(consumer, 2);

    producer.join();
    consumer.join();

    std::cout<<"debug"<<std::endl;

    std::sort(latencies.begin(), latencies.end());

    auto percentile = [&](double p) {
        return latencies[static_cast<size_t>(p * (latencies.size() - 1))];
    };

    long long sum = std::accumulate(latencies.begin(), latencies.end(), 0LL);


    std::cout << "\nLatency Benchmark Results (ns):\n";
    std::cout << "---------------------------------\n";
    std::cout << "Total messages: " << NUM_MESSAGES << "\n";
    std::cout << "Min latency:    " << latencies.front() << "\n";
    std::cout << "Max latency:    " << latencies.back() << "\n";
    std::cout << "Median:         " << percentile(0.5) << "\n";
    std::cout << "P95:            " << percentile(0.95) << "\n";
    std::cout << "P99:            " << percentile(0.99) << "\n";
    std::cout << "Avg:            " << sum / latencies.size() << "\n";

    return 0;
}
=======
#include <bits/stdc++.h>
#include <chrono>
#include <thread>
#include <mutex>

#include "RingBuffer.hpp"

using namespace std;
using namespace chrono;

// Time Utility
uint64_t now_ns() {
    return duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

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
                bool success= kv.second.push(msg);
                if(!success){
                    cout<<"[WARN] buffer is full for consumer "<<kv.first<<" on topic "<<topic<<endl;
                }
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

    //CONSUME IN BATCH
    bool consume_batch(int64_t topic, int64_t consumer_id, vector<Message>& out, size_t max_items) {
    lock_guard<mutex> lock(mtx);
    out.clear();
    auto t_it = subscriberQueue.find(topic);
    if (t_it != subscriberQueue.end()) {
        auto& map = t_it->second;
        auto c_it = map.find(consumer_id);
        if (c_it != map.end()) {
            Message msg;
            while (max_items-- && c_it->second.pop(msg)) {
                out.push_back(msg);
            }
            return !out.empty();
        }
    }
    return false;
}
};

// Benchmark parameters
const int NUM_MESSAGES = 10000;
const int64_t TOPIC = 1;
const int64_t CONSUMER_ID = 42;

int main() {
    MessageBroker broker;
    broker.subscribe(TOPIC, CONSUMER_ID);

    vector<uint64_t> latencies;

    thread producer([&]() {
        for (int i = 0; i < NUM_MESSAGES; ++i) {
            broker.publish(1, TOPIC, "msg_" + to_string(i));
            this_thread::sleep_for(microseconds(100));  // Simulate some load
        }
    });

    thread consumer([&]() {
        int received = 0;
        while (received < NUM_MESSAGES) {
            Message msg;
            if (broker.consume(TOPIC, CONSUMER_ID, msg)) {
                uint64_t recv_time = now_ns();
                uint64_t latency = recv_time - msg.timestamp;
                latencies.push_back(latency);
                ++received;
            } else {
                this_thread::sleep_for(microseconds(10));
            }
        }
    });

    producer.join();
    consumer.join();

    // Report Latency Stats
    sort(latencies.begin(), latencies.end());

    auto percentile = [&](double p) {
        return latencies[int(p * latencies.size())];
    };

    cout << "\nLatency Benchmark Results (ns):\n";
    cout << "Total messages: " << NUM_MESSAGES << "\n";
    cout << "Min latency: " << latencies.front() << "\n";
    cout << "Max latency: " << latencies.back() << "\n";
    cout << "Median: " << percentile(0.5) << "\n";
    cout << "P95: " << percentile(0.95) << "\n";
    cout << "P99: " << percentile(0.99) << "\n";
    cout << "Avg: " << accumulate(latencies.begin(), latencies.end(), 0ull) / latencies.size() << "\n";

    return 0;
}
>>>>>>> dffae0e2693bb4dee13003aabaf81b10ae33e2c7
