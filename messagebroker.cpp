////////// MESSAGE QUEUE///////////

#include <bits/stdc++.h>
#include <chrono>
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
    // TOPIC QUEUE
    unordered_map<int64_t, queue<Message>> topicQueue;

    // SUBSCRIBERS
    unordered_map<int64_t, vector<int64_t>> subscribers;

    // SUBSCRIBER QUEUE
    unordered_map<int64_t, unordered_map<int64_t, queue<Message>>> subscriberQueue; /// TOPIC -> CONSUMER ID -> QUEUE

public:
    void publish(int64_t type, int64_t topic, string data)
    {
        Message msg = {type, data, _timestamp()};
        for (auto [consumer_id, q] : subscriberQueue[topic])
        {
            q.push(msg);
        }
        cout << " topic " << topic << " " << " msg " << msg.payload << " type " << msg.type << " timestamp " << msg.timestamp << endl;
    }

    bool consume(int64_t topic, int64_t consumer_id, Message &out)
    {
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

    broker.publish(1, 1, "update=123");

    Message msg;
    if (broker.consume(1, 1001, msg))
        cout << "Consumer 1001 got: " << msg.payload << "\n";

    if (broker.consume(1, 1002, msg))
        cout << "Consumer 1002 got: " << msg.payload << "\n";
}
