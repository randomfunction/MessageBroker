////////// MESSAGE QUEUE///////////

#include <bits/stdc++.h>
#include <chrono>
#include <mutex>
#include <thread>

using namespace std;
using namespace chrono;

/// MESSAGE STRUCTURE
struct Message
{
    int64_t type;
    string payload;
    uint64_t timestamp; // HIGH PRECISION, LARGE RANGE, UNSIGNED
};

// TIMESTAMP
uint64_t _timestamp()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               std::chrono::high_resolution_clock::now().time_since_epoch())
        .count();
}

class MessageBroker
{
private:
    // MUTEX
    std::mutex mtx;

    // TOPIC QUEUE
    unordered_map<int64_t, queue<Message>> topicQueue;

    // SUBSCRIBERS
    unordered_map<int64_t, vector<int64_t>> subscribers;

    // SUBSCRIBER QUEUE
    unordered_map<int64_t, unordered_map<int64_t, queue<Message>>> subscriberQueue; /// TOPIC -> CONSUMER ID -> QUEUE

public:
    void publish(int64_t type, int64_t topic, string data)
    {
        std::lock_guard<std::mutex> lock(mtx);

        Message msg = {type, data, _timestamp()};
        for (auto [consumer_id, q] : subscriberQueue[topic])
        {
            q.push(msg);
        }
        cout << " topic " << topic << " " << " msg " << msg.payload << " type " << msg.type << " timestamp " << msg.timestamp << endl;
    }

    bool consume(int64_t topic, int64_t consumer_id, Message &out)
    {
        std::lock_guard<std::mutex> lock(mtx);

        if (subscriberQueue.find(topic) != subscriberQueue.end())
        {
            if (subscriberQueue[topic].find(consumer_id) != subscriberQueue[topic].end())
            {
                auto q = subscriberQueue[topic][consumer_id];
                if (!q.empty())
                {
                    out = q.front();
                    q.pop();
                    return true;
                }
            }
        }
        return false;
    }

    void subscribe(int topic, int consumer_id)
    {
        std::lock_guard<std::mutex> lock(mtx);

        if (subscriberQueue[topic].find(consumer_id) == subscriberQueue[topic].end())
        {
            subscriberQueue[topic][consumer_id] = queue<Message>();
        }
    }
};

signed main()
{
    MessageBroker broker;
    broker.subscribe(1, 1001);
    broker.subscribe(1, 1002);

    // PRODCUER THREAD
    auto producer = [&broker]()
    {
        for (int i = 0; i < 5; ++i)
        {
            broker.publish(1, 1, "msg_" + to_string(i));
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    };

    // CONSUMER THREAD
    auto consumer = [&broker](int id)
    {
        for (int i = 0; i < 5; ++i)
        {
            Message msg;
            if (broker.consume(1, id, msg))
            {
                cout << "Consumer " << id << " got: " << msg.payload << endl;
            }
            else
            {
                cout << "Consumer " << id << " no message\n";
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
