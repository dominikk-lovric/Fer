#include <cmath>
#include <iostream>
#include <stdio.h>
#include <signal.h>
#include <stack>
#include <string>
using namespace std;

int K_Z;
int T_P;
int ct;
stack<pair<pair<int, string>,int>> Stack;

void prt_zas(int j) {
    for (int i=0;i<4;i++) {
        int a=((j & ( 1 << (i) )) >> (i));
        cout<<a;
    }
}

void prt_stg(stack<pair<pair<int, string>,int>> S) {
    if (S.empty())cout<<"--------------------------------------";
    else {
        pair<pair<int, string>,int> pr;
        stack<pair<pair<int, string>,int>> S2;
        int z;
        string reg;
        while (!S.empty()) {
            pr=S.top();
            S.pop();
            S2.push(pr);
            prt_zas(1<<(pr.first.first-1));
            cout<<", "<<pr.first.second<<";";
        }
        while (!S2.empty()) {
            pr=S2.top();
            S2.pop();
            S.push(pr);
        }
    }
}
void prt_state(int i, int j, stack<pair<pair<int, string>,int>> S) {
    prt_zas(i);
    cout<<"     ";
    prt_zas(1<<j-1);
    cout<<"     ";
    prt_stg(S);
    cout<<endl;
}
void empty_stack(stack<pair<pair<int, string>,int>> &S) {
    while (!S.empty()) {
        S.pop();
    }
}

int put_bit(int i, int n) {
    i=i|1<<n;
    return i;
}

int bit_clear(int i, int n) {
    int c=~(1<<n);
    return i&c;
}

void obradi_signal(int sig){
    if (sig!=-1) {
        int i;
        cin>>i;
        K_Z=put_bit(K_Z,i-1);
        prt_state(K_Z,T_P,Stack);
        if (i>T_P) {
            Stack.push(pair<pair<int,string>,int>(pair<int,string>(T_P,"reg["+to_string(T_P)+"]"),ct));
            T_P=i;
            ct=5;
            K_Z=bit_clear(K_Z,i-1);
        }
        prt_state(K_Z,T_P,Stack);
    }else {
        cout<<endl;
        if (!Stack.empty()) {
            pair<pair<int, string>,int> pr;
            pr=Stack.top();
            Stack.pop();
            T_P=pr.first.first;
            ct=pr.second;
        }
        prt_state(K_Z,T_P,Stack);
        if (K_Z>0 && T_P<floor(log2(K_Z)+1)) {
            int i =floor(log2(K_Z))+1;
            Stack.push(pair<pair<int,string>,int>(pair<int,string>(T_P,"reg["+to_string(T_P)+"]"),ct));
            T_P=i;
            ct=5;
            K_Z=bit_clear(K_Z,i-1);
        }
        prt_state(K_Z,T_P,Stack);
    }
}

int main(void){

    struct sigaction act;
    act.sa_handler = obradi_signal;
    act.sa_flags = 0;
    act.sa_flags = SA_NODEFER;

    T_P=0;
    ct=-1;
    K_Z=0;
    empty_stack(Stack);

    sigaction(SIGINT, &act, NULL);

    printf("Moj pid je %d\n", getpid());

    prt_state(K_Z,T_P,Stack);
    cout<<endl;
    while(true){
        ct--;
        if (ct==0)obradi_signal(-1);
        sleep(1);
    }
    return 0;
}