#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <termios.h>
using namespace std;
struct sigaction prije;
vector<pair<pid_t, string>> pids;
bool skip;
void print_dir() {
    cout<<getcwd(NULL, 0);
}

vector<string> split(string line, char c) {
    vector<string> tokens;
    int last=0;
    for (int i=0; i<line.length(); i++) {
        if (line[i]==c) {
            string token=line.substr(last, i-last);
            last=i+1;
            tokens.push_back(token);
        }
    }
    if (last!=line.length()) {
        tokens.push_back(line.substr(last, line.length()
            -last));
    }
    return tokens;
}


void remove(std::vector<pair<pid_t,string>>& vec, std::size_t pos)
{
    vector<pair<pid_t,string>>::iterator it= vec.begin();
    std::advance(it, pos);
    vec.erase(it);
}

pid_t start_program(vector<string> tokens) {
    pid_t pid_novi;
    bool bg= (tokens[tokens.size()-1]=="&");
    if (bg) {
        tokens.pop_back();
    }
    if ((pid_novi = fork()) == 0) {
        sigaction(SIGINT, &prije, NULL);
        setpgid(pid_novi, pid_novi);
        if (!bg) {
            tcsetpgrp(STDIN_FILENO, getpgid(pid_novi));
        }
        char* args[tokens.size()+1];
        for (int i=0; i<tokens.size(); i++) {
            args[i] = tokens[i].data();
        }
        args[tokens.size()]=NULL;
		execvp(args[0], args);
		perror("Didn't start program");
		exit(1);
	}
	return pid_novi;
}

void obradi_dogadaj(int sig) {
    printf("\nProcess [%d] recieved signal %d\n", (int) getpid(), sig);
}

void obradi_dijete(int sig) {
    pid_t pid_zavrsio = waitpid(-1, NULL, WNOHANG);
    if (pid_zavrsio > 0){
        for (int i=0;i<pids.size();i++) {
            if (pids[i].first==pid_zavrsio) {
                remove(pids, i);
                cout<<"\nProgram ["<<pid_zavrsio<<"] finished\n";
                skip=1;
            }
        }
    }
}

int main() {
    skip=0;
    struct sigaction act;
    act.sa_handler = obradi_dogadaj;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, &prije);
    act.sa_handler = obradi_dijete;
    sigaction(SIGCHLD, &act, NULL);
    act.sa_handler = SIG_IGN;
    sigaction(SIGTTOU, &act, NULL);
    vector<string> history;
    pid_t novi;

    string line;

    struct termios shell_term_settings;
    pids.push_back(pair(getpid(), "main"));
    tcgetattr(STDIN_FILENO, &shell_term_settings);
    while (1) {
        cin.clear();
        if (skip==0) {
            print_dir();
            cout<<"~$ ";
            getline(cin, line);
            history.push_back(line);

        }else {
            skip=0;
        }
        vector<string> tokens = split(line, ' ');
        if (tokens.size()==0) {
            history.pop_back();
            continue;
        }
        if (tokens[0].compare("cd")==0) {
            if (chdir(tokens[1].c_str())==-1)cout<<"No such directory\n";
        }
        else if (tokens[0]=="pwd") {
            print_dir();
            cout<<endl;
        }
        else if (tokens[0].compare("ps")==0) {
            cout<<" pid     ime"<<endl;
            for (int i=0; i<pids.size(); i++) {
                cout<<pids[i].first<<"   "<<pids[i].second<<endl;
            }
        }
        else if (tokens[0].compare("kill")==0) {
            if (tokens.size()==3) {
                bool is=false;
                for (int i=0;i<pids.size();i++)if (pids[i].first==stoi(tokens[1]))is=true;
                if (is) {
                    kill(stoi(tokens[1]), stoi(tokens[2]));
                    line="";
                    cin.clear();
                }else cout<<"Process ["<< tokens[1] <<"] either doesn't exist or wasn't started by this shell\n";
            }else cout<<"Wrong arguments\n";
        }else if (tokens[0].compare("history")==0) {
            for (int i=0; i<history.size(); i++) {
                cout<<i+1<<"  "<<history[i]<<endl;
            }
        }
        else if (tokens[0].compare("exit")==0) {
            for (int i=1; i<pids.size(); i++) {
                cout<<"Killed process ["<<pids[i].first<<"]\n";
                kill(pids[i].first,SIGKILL);
            }
            cout<<"Exiting\n";
            exit(0);
        }
        else {
            if (tokens[0][0]=='!') {
                skip=1;
                string g =tokens[0];
                g.erase(0,1);
                line=history[stoi(g)-1];    
            }else{
                novi=start_program(tokens);
                pids.push_back(pair(novi, tokens[0]));
                if (tokens[tokens.size()-1]!="&") {
                    pid_t pid_zavrsio;
                    do {
                        pid_zavrsio = waitpid(novi, NULL, 0); 
                        if (pid_zavrsio > 0) {
                            if (kill(novi, 0) == -1) { 
                                for (int i=0;i<pids.size();i++) {
                                    if (pids[i].first==pid_zavrsio) {
                                        remove(pids, i);
                                    }
                                }

                                tcsetpgrp(STDIN_FILENO, getpgid(0));
                                tcsetattr(STDIN_FILENO, 0, &shell_term_settings);
                            }
                            else {
                                novi = (pid_t) 0; 
                            }
                        }
                        else {
                            printf("Waitpid finished but not providing information\n");
                            break;
                        }
                    }
                    while(pid_zavrsio <= 0);
                }
            }
        }
    }
    return 0;
}
