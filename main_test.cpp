#include "messagebroker.hpp"
#include <iostream>
#include <thread>
#include <vector>

int main() {
    MessageBroker broker;

    broker.subscribe(1, 1001);
    broker.subscribe(1, 1002);

    auto producer = [&broker]() {
        for (int i = 0; i < 10; ++i) {
            std::string data = "msg_" + std::to_string(i);
            std::cout << "Publishing: " << data << std::endl;
            broker.publish(1, 1, data);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    };

    auto consumer = [&broker](int64_t id) {
        for (int i = 0; i < 10; ++i) {
            Message msg;
            if (broker.consume(1, id, msg)) {
                std::cout << "Consumer " << id << " got: " << msg.payload
                          << " at " << msg.timestamp << std::endl;
            } else {
                std::cout << "Consumer " << id << " has no message" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
    };

    std::thread t1(producer);
    std::thread t2(consumer, 1001);
    std::thread t3(consumer, 1002);

    t1.join();
    t2.join();
    t3.join();

    return 0;
}