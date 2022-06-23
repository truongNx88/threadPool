/**
 * @file ThreadPool.hpp
 * @author Nguyen Xuan Truong
 * @brief 
 * @version 0.1
 * @date 2022-06-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef ThreadPool_hpp
#define ThreadPool_hpp

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <condition_variable>

class ThreadPool {
private:
    void threadLoop();

    bool shouldTerminate = false; // Tells threads to stop looking for jobs
    std::mutex queueMutex; // Prevents data races to the job queue
    std::condition_variable mutexCondition; // Allows threads to wait on new jobs or termination 
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> jobs;
    int maxThread; // max number of threads allowed to run at the same time 
public:
    ThreadPool();
    ~ThreadPool();
    void setMaxThread(int maxThread);
    void start();
    void queueJob(const std::function<void()>& job);
    void stop();
    bool busy();
};
#endif

