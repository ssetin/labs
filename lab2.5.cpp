#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

using namespace std;
mutex coutmutex;


class A{
    once_flag fl;
    mutable mutex mut;
    bool Initialized;
    int sum;
    void Init(){
       Initialized=true;
       sum=0;
    }
public:
    A(){
        Initialized=false;
        sum=-1;
    }
    ~A()=default;
    A(const A &a)=delete;
    A& operator=(const A &a)=delete;

    A(const A &&a){
        lock_guard<mutex> alock(a.mut);
        sum=move(a.sum);
        Initialized=move(a.Initialized);
    }

    A& operator=(const A &&a){
        if(this==&a)
            return *this;
        unique_lock<mutex> alock(a.mut);
        unique_lock<mutex> tlock(mut);
        lock(alock, tlock);
        sum=move(a.sum);
        Initialized=move(a.Initialized);
        return *this;
    }

    void Add(int dsum){
        call_once(fl, &A::Init, this);
        lock_guard<mutex> thlock(mut);
        sum+=dsum;
    }
    int GetSum(){
        lock_guard<mutex> thlock(mut);
        return sum;
    }
};


void multisum(unique_ptr<A> &a){
    (*a).Add(1);
}


void multisum2(){
    unique_ptr<A> a(new A());
    int times(3);

    while(times>0){
        (*a).Add(2);
        times--;
    }
    lock_guard<mutex> lockm(coutmutex);
    cout<<"Thread "<<this_thread::get_id()<<" sum =  "<<(*a).GetSum()<<endl;
}



int main()
{
    //1
    vector<thread> vth;
    unique_ptr<A> oneAptr(new A());

    for(int i=0;i<5;i++){
         vth.emplace_back(thread(multisum, ref(oneAptr)));
    }
    for(thread &t: vth){
        t.join();
    }
    vth.clear();
    cout<<"Sum 1 =  "<<(*oneAptr).GetSum()<<endl;

    //2
    for(int i=0;i<5;i++){
         vth.emplace_back(thread(multisum2));
    }
    for(thread &t: vth){
        t.join();
    }
    vth.clear();



    cout<<"Fin.";
    cin.get();
    return 0;
}
