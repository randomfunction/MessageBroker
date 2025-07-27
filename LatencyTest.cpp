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