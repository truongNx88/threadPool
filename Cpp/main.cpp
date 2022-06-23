#include "ThreadPool.hpp"
#include <unistd.h>

void printTask(std::string threadName) {
    for (size_t i = 0; i < 5; i++) {
        if (i == 0 ) {
            std::cout << "Start thread: " << threadName << std::endl;
        }
        else {
            std::cout << "Running thread: " << threadName << std::endl;
        }
        sleep(1);
    }
    std::cout << threadName << " complete" << std::endl;
}

int main(int argc, char const *argv[]) {
    ThreadPool* pool = new ThreadPool();
    // pool->setMaxThread(3);
    
    for (size_t i = 1; i < 6; i++) {
        std::string threadName = "Thread " + std::to_string(i);
        pool->queueJob( [&] {
            printTask(threadName);
        });
    }
    pool->start();
    
    pool->stop();
    return 0;
}
