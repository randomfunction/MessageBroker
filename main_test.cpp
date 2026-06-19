#include "messagebroker.hpp"
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

int main() {
    MessageBroker broker;

    broker.subscribe(1, 1001);
    broker.subscribe(1, 1002);

    auto producer = [&broker]() {
        for (int i = 0; i < 10; ++i) {
            string data = "msg_" + to_string(i);
            cout << "Publishing: " << data << endl;
            broker.publish(1, 1, data);
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    };

    auto consumer = [&broker](int64_t id) {
        for (int i = 0; i < 10; ++i) {
            Message msg;
            if (broker.consume(1, id, msg)) {
                cout << "Consumer " << id << " got: " << msg.payload
                          << " at " << msg.timestamp << endl;
            } else {
                cout << "Consumer " << id << " has no message" << endl;
            }
            this_thread::sleep_for(chrono::milliseconds(150));
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