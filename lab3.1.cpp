#include<iostream>
#include<atomic>
#include<thread>
#include<mutex>
#include<vector>
#include<string>
#include<chrono>

using namespace std;

class spinlock {
	atomic_flag flag{ ATOMIC_FLAG_INIT };
public:
	spinlock()=default;
	spinlock(const spinlock& s) = delete;
	spinlock& operator=(const spinlock& s) = delete;	
	~spinlock() {
		flag.clear();
	}
	void lock() {
		while (flag.test_and_set()) { this_thread::yield(); }
	}
	void unlock() {
		flag.clear();
	}
	bool try_lock() {
		return flag.test_and_set();
	}
};

template<typename T, typename TMutex>
class threadsafe_stack {
	vector<T> top;
	mutable TMutex mut;
public:
	threadsafe_stack() = default;

	threadsafe_stack(const threadsafe_stack &from) {
		lock_guard<TMutex> fromlock(from.mut);
		top = from.top;
	}
	threadsafe_stack& operator=(const threadsafe_stack &from) {
		if (this == &from)
			return *this;
		unique_lock<TMutex> thislock(mut, defer_lock);
		unique_lock<TMutex> fromlock(from.mut, defer_lock);
		lock(thislock, fromlock);
		top = from.top;
		return *this;
	}

	threadsafe_stack(threadsafe_stack &&from) {
		lock_guard<TMutex> fromlock(from.mut);
		top = move(from.top);
	}

	threadsafe_stack& operator=(threadsafe_stack &&from) {
		if (this == &from)
			return *this;
		unique_lock<TMutex> thislock(mut, defer_lock);
		unique_lock<TMutex> fromlock(from.mut, defer_lock);
		lock(thislock, fromlock);
		top = move(from.top);

		return *this;
	}

	~threadsafe_stack() {
		top.clear();
	}

	void push(T val) {
		lock_guard<TMutex> thislock(mut);
		top.push_back(val);
	}

	shared_ptr<T> pop() {
		lock_guard<TMutex> thislock(mut);
		if (top.empty()) {
			throw "stack is empty";
		}
		shared_ptr<T> res = make_shared<T>(top.back());
		top.pop_back();
		return res;
	}

	void pop(T &val) {
		lock_guard<TMutex> thislock(mut);
		if (top.empty()) {
			throw "stack is empty";
		}
		val = move_if_noexcept(top.back());
		top.pop_back();
	}

	bool empty() {
		lock_guard<TMutex> thislock(mut);
		return top.empty();
	}
};

template<typename T>
void writer(T &s, int val, int times) {
	while (times>0) {
		s.push(val);
		times--;
	}
}

template<typename T>
void reader(T &s) {
	while (!s.empty()) {
		try {
			shared_ptr<int> val = s.pop();
			//cout << *val << " ";
		}
		catch (const char *msg) {
			cout << msg << endl;
		}
	}
}




int main() {
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	chrono::duration<double> els,els2;

	using threadsafe_stack_mutex = threadsafe_stack<int, mutex>;
	using threadsafe_stack_spinlock = threadsafe_stack<int, spinlock>;
	threadsafe_stack_mutex stthread;
	threadsafe_stack_spinlock st;

	vector<thread> vth;


	//1
	start = chrono::high_resolution_clock::now();
	for (int i = 0;i<10;i++) {
		vth.emplace_back(thread(writer<threadsafe_stack_mutex>, ref(stthread), i, 300000));
	}
	thread threader(reader<threadsafe_stack_mutex>, ref(stthread));
	for (thread &t : vth) {
		t.join();
	}
	vth.clear();
	threader.join();
	end = chrono::high_resolution_clock::now();
	els = end - start;
	

	//2
	start = chrono::high_resolution_clock::now();
	for (int i = 0;i<10;i++) {
		vth.emplace_back(thread(writer<threadsafe_stack_spinlock>, ref(st), i, 300000));
	}

	thread threader2(reader<threadsafe_stack_spinlock>, ref(st));
	for (thread &t : vth) {
		t.join();
	}
	vth.clear();
	threader2.join();
	end = chrono::high_resolution_clock::now();
	els2 = end - start;

	cout << "Milliseconds (mutex) " << els.count() << endl;
	cout << "Milliseconds (spinlock) " << els2.count() << endl;

	
	cout << "Fin.";
	cin.get();
	return 0;
}
