#include<iostream>
#include<thread>
#include<algorithm>
#include<chrono>
#include<vector>

using namespace std;


template <typename T>
void transformme(T &in, T &out, int bg, int sz) {
    out.resize(in.size());
    typename T::iterator initer=in.begin()+bg, outiter=out.begin()+bg;
    transform(initer, initer+sz , outiter, [](decltype(*initer) &ii) {return abs(ii);});
}


int main() {

    int corecount= thread::hardware_concurrency();
    int thcount(0);
    vector<thread> vth;
    std::chrono::time_point<std::chrono::system_clock> start, end;

    vector<int> arr{1, 4, 6, 8, -3, -3, -5 ,6 ,2, -3, 9, 88, -55};
    vector<int> outarr;

    cout << "Core count: " << corecount << endl;
    cout<<"Enter threads count: "; cin >> thcount;
    //thcount = 3;
    if (thcount > 1)
        thcount--;

    start = chrono::system_clock::now();

    int ebp = arr.size() / thcount;
    for (int i = 0; i < thcount;i++) {
        vth.emplace_back(thread(transformme<decltype(arr)>, std::ref(arr), std::ref(outarr), i*ebp, ebp));
    }

    if(thcount*ebp<arr.size())
       transformme<decltype(arr)>(std::ref(arr), std::ref(outarr),thcount*ebp,arr.size()-thcount*ebp);

    for (thread &th : vth) {
        th.join();
    }

    end = chrono::system_clock::now();
    chrono::duration<double> els = end - start;
    cout << "Milliseconds " << els.count() << endl;

    for (auto &a : outarr) {
        cout << a << " ";
    }
    cout << endl;
    cout << "Fin." << endl;

    return 0;
}
