#include<iostream>
#include<thread>
#include<mutex>
#include<vector>
#include <sstream>
#include<string>
#include <condition_variable>

using namespace std;

struct task {
    string message;
    task() : message("") {}
    task(string msg) : message(msg) {}
};

class thsQueue {
    vector<task> tasks;
    mutable mutex mut;
    condition_variable cvar;
public:
    thsQueue()=default;
    ~thsQueue()=default;

    thsQueue(const thsQueue& from){
        lock_guard<mutex> thlock(from.mut);
        tasks=from.tasks;
    }

    thsQueue& operator=(const thsQueue &from){
            if(this==&from)
                return *this;
            unique_lock<mutex> thislock(mut, defer_lock);
            unique_lock<mutex> fromlock(from.mut, defer_lock);
            lock(thislock, fromlock);
            tasks=from.tasks;
            return *this;
    }

    void NewTask(string msg) {
        lock_guard<mutex> thlock(mut);
        tasks.emplace(tasks.begin(),task(msg));
        cvar.notify_all();
    }
    void NewTask(task newtask) {
        lock_guard<mutex> thlock(mut);
        tasks.insert(tasks.begin(),newtask);
        cvar.notify_all();
    }
    shared_ptr<task> GetReadyTask() {
        unique_lock<mutex> thlock(mut);
        cvar.wait(thlock, [this](){ return !tasks.empty(); });
        shared_ptr<task> res=make_shared<task>(tasks.back());
        tasks.pop_back();
        return res;
    }

};

void writer(thsQueue &thq, string msg=""){
    srand(time(NULL));
    int times=3;//rand()%5+1;
    while(times>0){
        this_thread::sleep_for(chrono::milliseconds(rand()%2000+2000));
        stringstream ss;
        ss<<this_thread::get_id();
        string sid = ss.str();
        if(msg!="exit")
            msg="fix bug "+sid+to_string(rand()%10);
        thq.NewTask(msg);
        times--;
    }
}

void reader(thsQueue &thq){
    while(1){
        shared_ptr<task> t=thq.GetReadyTask();
        if(t){
            if(task(*t).message=="exit"){
                cout<<"Exit!"<<endl;
                break;
            }
            else
                cout<<"Task "<<task(*t).message<<" ready"<<endl;
        }
    }
}


int main() {
    thsQueue thq;
    vector<thread> vth;

    thread serverth(reader, ref(thq));

    for(int i=0;i<4;i++){
        vth.emplace_back(thread(writer, ref(thq), ""));
    }

    for(thread &t: vth){
        t.join();
    }

    writer(thq,"exit");

    serverth.join();

    cout << "Fin." << endl;
    cin.get();
    return 0;
}
