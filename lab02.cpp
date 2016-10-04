#include<iostream>
#include<thread>
#include<vector>
#include<Windows.h>
#include<chrono>

using namespace std;


void beepme(int fr, int delay) {
    Beep(fr,delay);
}

void PlayChord(int ch[],int delay){
    int te(0);
    while(te<delay){
        Beep(ch[0],1);
        Beep(ch[1],1);
        Beep(ch[2],1);
        te++;
    }
}


int main() {
    int sounds[] = {261, 293, 329, 349, 392, 440, 493};
    int Gm[]={392, 293, 233};
    vector<thread> vth;

    cout << "2a" << endl;
    for (int &t : sounds) {
        vth.emplace_back(thread(beepme, t, 320));
    }

    for (thread &th : vth) {
        th.detach();
    }

    this_thread::sleep_for(chrono::seconds(3));

    cout<<"2b"<<endl;

    PlayChord(Gm, 100);



    cout << "Fin." << endl;

    return 0;
}
