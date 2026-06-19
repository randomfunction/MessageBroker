#include "messagebroker.hpp"
#include <iostream>
#include <cstring>

using namespace std;

void MessageBroker::subscribe(int64_t topic_id, int64_t consumer_id) {
    shared_ptr<TopicData> topic_data;
    {
        lock_guard<mutex> lock(map_mtx);
        if (topics.find(topic_id) == topics.end()) {
            topics[topic_id] = make_shared<TopicData>();
        }
        topic_data = topics[topic_id];
    }

    lock_guard<mutex> lock(topic_data->topic_mtx);
    topic_data->subscribers.try_emplace(consumer_id);
}

void MessageBroker::publish(int64_t type, int64_t topic_id, const std::string& data) {
    shared_ptr<TopicData> topic_data;
    {
        lock_guard<mutex> lock(map_mtx);
        auto it = topics.find(topic_id);
        if (it == topics.end()) {
            return; 
        }
        topic_data = it->second;
    } 

    Message msg;
    msg.type = type;
    msg.timestamp = getCurrentTimestamp();
    strncpy(msg.payload, data.c_str(), sizeof(msg.payload) - 1);
    msg.payload[sizeof(msg.payload) - 1] = '\0'; 

    lock_guard<mutex> lock(topic_data->topic_mtx);
    for (auto& pair : topic_data->subscribers) {
        if (!pair.second.push(msg)) {
            cerr << "[WARN] Buffer is full for consumer " << pair.first << " on topic " << topic_id << endl;
        }
    }
}

bool MessageBroker::consume(int64_t topic_id, int64_t consumer_id, Message& out) {
    shared_ptr<TopicData> topic_data;
    {
        lock_guard<mutex> lock(map_mtx);
        auto it = topics.find(topic_id);
        if (it == topics.end()) {
            return false;
        }
        topic_data = it->second;
    } 

    lock_guard<mutex> lock(topic_data->topic_mtx);
    auto consumer_it = topic_data->subscribers.find(consumer_id);
    if (consumer_it != topic_data->subscribers.end()) {
        return consumer_it->second.pop(out);
    }
    return false;
}

bool MessageBroker::consume_batch(int64_t topic_id, int64_t consumer_id, vector<Message>& out, size_t max_items) {
    out.clear();
    shared_ptr<TopicData> topic_data;
    {
        std::lock_guard<std::mutex> lock(map_mtx);
        auto it = topics.find(topic_id);
        if (it == topics.end()) {
            return false;
        }
        topic_data = it->second;
    }

    out.reserve(max_items);

    lock_guard<mutex> lock(topic_data->topic_mtx);
    auto consumer_it = topic_data->subscribers.find(consumer_id);
    if (consumer_it != topic_data->subscribers.end()) {
        Message msg;
        while (max_items-- > 0 && consumer_it->second.pop(msg)) {
            out.push_back(msg);
        }
        return !out.empty();
    }
    return false;
}