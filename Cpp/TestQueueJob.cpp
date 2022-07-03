#include <iostream>
#include <functional>
#include <unistd.h>
#include <queue>

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
    std::queue<std::function<void()>> jobs;

    for (size_t i = 0; i < 3; i++) {
        jobs.push([=]() -> void {
            std::string name = "Check " + std::to_string(i);
            std::cout << "Name: " << name << std::endl;
            printTask(name);
        });
    }

    std::cout << std::endl << "Queue run" << std::endl;

    for (size_t i = 0; i < 3; i++) {
        std::function<void()> job = jobs.front();
        jobs.pop();
        job();
    }
    
    std::cout << "End test" << std::endl;
    
    return 0;
}
