#include<iostream>
#include<vector>
#include<algorithm>
#include<chrono>
#include<numeric>
#include<future>


using namespace std;

int accme(vector<int> &v, int bg, int sz) {
	return accumulate(v.begin()+bg, v.begin() + bg + sz, 0);
}

void Process(int corecount, vector<int> &vres) {
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	vector<future<int>> vfut;	
	int ebp = vres.size() / corecount;
	int res0(0);
	start = chrono::high_resolution_clock::now();
	for (int i = 0; i < corecount;i++) {
		vfut.emplace_back(async(std::launch::async, accme, ref(vres), i*ebp, ebp));
	}

	if (corecount*ebp<vres.size())
		res0 = accumulate(vres.begin() + corecount*ebp, vres.end(), 0);

	for (auto &t : vfut) {
		res0 += t.get();
	}
	res0 = res0 / vres.size();
	end = chrono::high_resolution_clock::now();
	chrono::duration<double> els = end - start;
	cout << "Threads: " << corecount << endl;
	cout << "Result: " << res0 << endl;
	cout << "Milliseconds " << els.count() << endl;
	cout << "*******************************************" << endl;

}


int main() {
	std::chrono::time_point<std::chrono::system_clock> start, end;	
	vector<int> vres;
	

	for (int i=0; i < 1000000; i++) {
		vres.push_back(i);
	}

	start = chrono::system_clock::now();
	int result = accumulate(vres.begin(), vres.end(), 0)/vres.size();
	end = chrono::system_clock::now();
	chrono::duration<double> els = end - start;
	cout << "Result: " << result<<endl;
	cout << "Milliseconds " << els.count() << endl;


	int corecount = thread::hardware_concurrency();
	cout << "Cores: " << thread::hardware_concurrency() << endl<<endl;

	Process(corecount-1, vres);
	Process(corecount + 2, vres);
	



	cout << "Fin." << endl;
	cin.get();

	return 0;
}
