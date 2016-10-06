#include<iostream>
#include<future>
#include<string>


using namespace std;

int load() {
	cout << "Enter int value: ";
	int a(0);
	string str;
	try {
		cin >> str;
		a = stoi(str);
	}
	catch (const invalid_argument& e) {
		throw e;
	}
	catch (const std::out_of_range& e) {
		throw e;
	}
	catch (...) {
		throw "error";
	}
	return a;
}


int main() {
	
	future<int> f = async(load);

	try {
		cout<< f.get()<<endl;
	}
	catch(const invalid_argument& e){
		cout << e.what() << endl;
	}
	catch (const std::out_of_range& e) {
		cout << e.what() << endl;
	}
	catch (...) {
		cout << "error" << endl;
	}

	cout << "Fin." << endl;
	cin.get();
	return 0;

}
