#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

using namespace std;


typedef struct{
    char M[5];
    int ULAZ;
    int IZLAZ;
    int strNum;
    sem_t pisi;
    sem_t poruke;
    sem_t pretinci;
}shrData;

shrData* shared;

int strNum;

void proizvodac(string str, int id){
    for (char c : str)
    {
        sem_wait(&shared->pretinci);
        sem_wait(&shared->pisi);

        shared->M[shared->ULAZ] = c;
        shared->ULAZ = (shared->ULAZ + 1) % 5;
        cout<<"proizvodac"<<id<<" -> "<<c<<endl;
        sem_post(&shared->pisi);
        sem_post(&shared->poruke);
        sleep(1);
    }
    sem_wait(&shared->pretinci);
    sem_wait(&shared->pisi);
    char c='\0';
    shared->M[shared->ULAZ] = c;
    cout<<"proizvodac"<<id<<" -> "<<c<<endl;
    shared->ULAZ = (shared->ULAZ + 1) % 5;

    sem_post(&shared->pisi);
    sem_post(&shared->poruke);
    sleep(1);
}

void potrosac(){
    string result;
    int fin=0;
    while (fin < shared->strNum){
        sem_wait(&shared->poruke);

        char c = shared->M[shared->IZLAZ];
        shared->IZLAZ = (shared->IZLAZ + 1) % 5;
        sem_post(&shared->pretinci);
        cout << "potrosac <- " << c << endl;
        if (c == '\0') {
            fin++;
        }
        else {
            result += c;
        }
    }

    cout << "Received: " << result << endl;
}

int main(int argc, const char * argv[]) {
    int shmid = shmget(IPC_PRIVATE,sizeof(shrData),IPC_CREAT | 0666);
    shared =(shrData*) shmat(shmid, NULL, 0);

    shared->strNum=argc-1;
    sem_init(&shared->pretinci, 1, 5);
    sem_init(&shared->pisi, 1, 1);
    sem_init(&shared->poruke, 1, 0);
    shared->ULAZ = 0;
    shared->IZLAZ = 0;

    for (int i = 0; i < argc-1; i++) {
        if (fork() == 0) {
            proizvodac(argv[i+1], i);
            exit(0);
        }
    }
    if (fork()==0) {
        potrosac();
        exit(0);
    }

    for (int i = 0; i < argc; i++)
    {
        wait(NULL);
    }

    sem_destroy(&shared->pretinci);
    sem_destroy(&shared->pisi);
    sem_destroy(&shared->poruke);
    shmdt(shared);
    shmctl(shmid, IPC_RMID, NULL);

}