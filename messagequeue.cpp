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

queue<Message> msgq;

// TIMESTAMP
uint64_t _timestamp() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}


// PRODUCER
void produce(int64_t type, string data){
    Message msg={type,data,_timestamp()};
    msgq.push(msg);
    cout<<"produced "<<msg.payload<<endl;
}

// CONSUMER
void consume(){
    if(!msgq.empty()){
        Message msg= msgq.front();
        msgq.pop();
        cout<<"consumed "<<msg.payload<<" "<<"time "<<msg.timestamp<<endl;
    }
    else{
        cout<<"no message to read"<<endl;
    }
}

signed main(){
    produce(1,"hello");
    consume();
}





