#include "ThreadPool.hpp"

ThreadPool::ThreadPool() {
    this->shouldTerminate = false;
    this->maxThread = std::thread::hardware_concurrency(); //// Max # of threads the system supports
}

ThreadPool::~ThreadPool() {
}

void ThreadPool::setMaxThread(int maxThreads) {
    this->maxThread = maxThreads;
}

// Each thread should be running its own infinite loop, constantly waiting for new tasks to grab and run.
void ThreadPool::start() {
    this->threads.resize(this->maxThread);
    std::cout << "Starting pool with " << this->maxThread << " threads." << std::endl;
    for (size_t i = 0; i < this->maxThread; i++) {
        std::cout << "Start thread at " << i << std::endl;
        threads.at(i) = std::thread(&ThreadPool::threadLoop, this);
    }
}

//The infinite loop function. This is a while (true) loop waiting for the task queue to open up.
void ThreadPool::threadLoop() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(this->queueMutex);
            std::cout << "Size jobs: " << jobs.size() << std::endl;
            std::cout << "shouldTerminate: " << this->shouldTerminate << std::endl;
            mutexCondition.wait(lock, [this]{
                return !jobs.empty() || this->shouldTerminate;
            });
            if (this->shouldTerminate) {
                return ;
            }

            job = this->jobs.front();
            this->jobs.pop();
        }
        job();   
    } 
}

void ThreadPool::queueJob(const std::function<void()>& job) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        jobs.push(job);
    }
    mutexCondition.notify_one();
}

/*The busy() function can be used in a while loop, 
such that the main thread can wait the threadpool to complete all the tasks before calling the threadpool destructor.*/
bool ThreadPool::busy() {
    bool poolBusy;
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        poolBusy = jobs.empty();
    }
    return poolBusy;
}

/* Once you integrate these ingredients, you have your own dynamic threading pool. 
These threads always run, waiting for job to do. */
void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        this->shouldTerminate = true;
    }
    mutexCondition.notify_all();
    for (std::thread &activeThread : this->threads) {
        std::cout << "Join Thread" << std::endl;
        activeThread.join();
    }
    threads.clear();
}