#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include <mutex>

using namespace std;


template<typename T>
class threadsafe_stack{
    vector<T> top;
    mutable mutex mut;
public:
    threadsafe_stack()=default;

    threadsafe_stack(const threadsafe_stack &from){        
        lock_guard<mutex> fromlock(from.mut);
        top=from.top;
    }
    threadsafe_stack& operator=(const threadsafe_stack &from){
        if(this==&from)
            return *this;
        unique_lock<mutex> thislock(mut, defer_lock);
        unique_lock<mutex> fromlock(from.mut, defer_lock);
        lock(thislock, fromlock);
        top=from.top;        
        return *this;
    }

    threadsafe_stack(threadsafe_stack &&from){
        lock_guard<mutex> fromlock(from.mut);
        top=move(from.top);
    }

    threadsafe_stack& operator=(threadsafe_stack &&from){
        if(this==&from)
            return *this;
        unique_lock<mutex> thislock(mut, defer_lock);
        unique_lock<mutex> fromlock(from.mut, defer_lock);
        lock(thislock, fromlock);
        top=move(from.top);

        return *this;
    }

    ~threadsafe_stack(){
        top.clear();
    }
    void push(T val){
        lock_guard<mutex> thislock(mut);
        top.push_back(val);
    }

    shared_ptr<T> pop(){
        lock_guard<mutex> thislock(mut);
        if(top.empty()){
            throw "stack is empty";
        }
        shared_ptr<T> res=make_shared<T>(top.back());
        top.pop_back();
        return res;
    }

    void pop(T &val){
        lock_guard<mutex> thislock(mut);
        if(top.empty()){
            throw "stack is empty";
        }
        val=move_if_noexcept(top.back());
        top.pop_back();
    }

    bool empty(){
        lock_guard<mutex> thislock(mut);
        return top.empty();
    }
};


void writer(threadsafe_stack<int>& s, int val, int times){
    while(times>0){
        s.push(val);
        this_thread::sleep_for(chrono::milliseconds(20));
        times--;
    }
}

void reader(threadsafe_stack<int>& s){
    while(!s.empty()){
        try{
            shared_ptr<int> val=s.pop();
            cout<<*val<<endl;
        }catch(const char *msg){
            cout<<msg<<endl;
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}


int main()
{
    threadsafe_stack<int> st;
    vector<thread> vth;

    st.push(100);

    threadsafe_stack<int> st2=move(st);

    for(int i=0;i<10;i++){
        vth.emplace_back(thread(writer,ref(st),i, 3));
    }

    thread threader(reader,ref(st));

    for(thread &t:vth){
        t.join();
    }
    vth.clear();

    threader.join();

    cout<<"Fin.";
    cin.get();
    return 0;
}
