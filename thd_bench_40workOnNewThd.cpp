#include <cstring>
#include <mutex>
#include <thread>
#include <vector>
#include <unistd.h>
#include <atomic>
#include <condition_variable>
#include <cstdlib>
#include <cstdio>
#include<fcntl.h>
#include<errno.h>
#include <queue>
#include <time.h>


static inline unsigned long long nanoseconds()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts); //CLOCK_MONOTONIC
    return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}



std::mutex mu;
//std::condition_variable cv_for_connected;
std::condition_variable cv;
std::atomic<int> finish;
char *b = new char[1000000];
static void work0(std::atomic<int> *finish, char *b) {
    for(int i = 0;i<20;i++) {
        nanoseconds();
        char * a = new char[1000000];
    
        memcpy(a, b, 1000000);
        delete []a;
    //   usleep(10000);
        nanoseconds();
     }
    //  delete[]b;
}

static void work(std::atomic<int> *finish, char *b) {
    while(!(*finish)) {
        //work0(finish, b);
        auto thd = std::make_shared<std::thread>(work0, finish, b);
        thd->join();
    }
}

static void wait_task(std::condition_variable *cv, std::atomic<int> *finish) {
    while(true) {
        //usleep(10000);
         std::unique_lock<std::mutex> lk(mu);

         cv->wait(lk, [&]() { 
             int ret = *finish;
             return ret; 
             });
    }
}

int main() {
    finish=false;
    std::vector<std::thread*> thds;
    std::vector<std::shared_ptr<std::thread>> wthds;
    //for(int i=0;i<1000;i++) {
    //    thds.push_back(new std::thread(wait_task, &cv, &finish));
    //}
    for(int i = 0;i<40;i++) {
        wthds.push_back(std::make_shared<std::thread>(work, &finish, b));
    }
    for(auto &thd: wthds) thd->join();
    // while(true) {
    //     auto &thd = wthds.front();
    //     wthds.pop();
    //     thd->join();
    //     wthds.push_back(std::make_shared<std::thread>(work, &finish, b));
    // }
    
    return 0;
}
