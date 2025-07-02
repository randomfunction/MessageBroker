////////// MESSAGE QUEUE///////////

#include<bits/stdc++.h>
#include<chrono>
using namespace std;
using namespace chrono;


/// MESSAGE STRUCTURE 
struct Message{
    int64_t type;
    string payload;
    uint64_t timestamp;   // HIGH PRECISION, LARGE RANGE, UNSIGNED
};

// TIMESTAMP
uint64_t _timestamp() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

class MessageBroker{
    private:

    // TOPIC QUEUE
    unordered_map<int64_t, queue<Message>> topicQueue;

    //SUBSCRIBERS
    unordered_map<int64_t, vector<int64_t>> subscribers;

    public:
    void publish(int64_t type, int64_t topic, string data){
        Message msg={type, data, _timestamp()};
        topicQueue[topic].push(msg);
        cout<<"topic "<<topic<<" "<<"msg "<<msg.payload<<"type "<<msg.type<<"timestamp "<<msg.timestamp<<endl;
    }

    bool consume(int topic, Message& out){
        if(topicQueue.find(topic)!=topicQueue.end()){
            if(!topicQueue[topic].empty()){
                out = topicQueue[topic].front();
                topicQueue[topic].pop();
                return true;
            }
        }
        return false;
    }

    void subscribe(int topic, int consumer_id){
        subscribers[topic].push_back(consumer_id);
    }
};



