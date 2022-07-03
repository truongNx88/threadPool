#include "ThreadPool.hpp"

ThreadPool::ThreadPool() {
    this->shouldTerminate = false;
    this->maxThread = 0;
}

ThreadPool::~ThreadPool() {
}

void ThreadPool::setMaxThread(int maxThreads) {
    this->maxThread = maxThreads;
}

// Each thread should be running its own infinite loop, constantly waiting for new tasks to grab and run.
void ThreadPool::start() {
    if (this->maxThread <= 0 ) {
        this->maxThread = std::thread::hardware_concurrency(); // Max # of threads the system supports
    }
    this->threads.resize(this->maxThread);
    for (size_t i = 0; i < this->maxThread; i++) {
        this->threads.at(i) = std::thread(&ThreadPool::threadLoop, this);
    }
}

//The infinite loop function. This is a while (true) loop waiting for the task queue to open up.
void ThreadPool::threadLoop() {
    int count = 0;
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(this->queueMutex);
            this->mutexCondition.wait(lock, [this]{
                return !this->jobs.empty() || this->shouldTerminate;
            });
            if (this->shouldTerminate) {
                return ;
            }
            this->shouldTerminate = this->jobs.empty();
            job = this->jobs.front();
            this->jobs.pop();
        }
        job();   
    } 
}

void ThreadPool::queueJob(const std::function<void()>& job) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        this->jobs.push(job);
    }
    mutexCondition.notify_one();
}

/*The busy() function can be used in a while loop, 
such that the main thread can wait the threadpool to complete all the tasks before calling the threadpool destructor.*/
bool ThreadPool::busy() {
    bool poolBusy;
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        poolBusy != jobs.empty();
    }
    return poolBusy;
}

/* Once you integrate these ingredients, you have your own dynamic threading pool. 
These threads always run, waiting for job to do. */
void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);        
        // this->shouldTerminate = true;
    }
    mutexCondition.notify_all();
    for (std::thread &activeThread : this->threads) {
        activeThread.join();
    }
    this->threads.clear();
}