#include<iostream>
#include<future>
#include<string>
#include<mutex>

using namespace std;

mutex mut;

struct TRes {
	string name;
	int acount;
	int order;
	void print() { cout <<order<< ". NaME: " << name << ", A count: " << acount << endl; }
};

class A {
	int m_a;	
public:	
	static thread_local int count;
	A() { count++; }
	static int GetCount() { return count; }
};

thread_local int A::count = 0;

TRes thfunc(string thname, int acount) {
	static int order = 0;
	A::count = 0;		
	lock_guard<mutex> l(mut);
	order++;
	TRes r;

	while (acount > 0) {
		A newa;
		acount--;
	}		

	r.name = thname;
	r.acount = A::GetCount();
	
	r.order=order;
	return r;
}


int main() {
	vector<future<TRes>> fvec;
	srand(time(NULL));

	for (int i = 0;i < 5; i++) {
		fvec.emplace_back(async(std::launch::async, thfunc, "thread " + std::to_string(i + 1), rand() % 10 + 1) );
	}

	cout << "Results: " << endl;

	for (auto &t : fvec) {
		t.get().print();
	}



	cin.get();

	return 0;
}
