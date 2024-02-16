//
// Created by lhc456 on 2024/2/16.
//

#include "threadpool.h"
#include <iostream>
#include <thread>

const int TASK_MAX_THRESHHOLD = INT32_MAX;
const int THREAD_MAX_THRESHHOLD = 1024;
const int THREAD_MAX_IDLE_TIME = 60; // 单位：秒


// 线程池构造
ThreadPool::ThreadPool()
        : initThreadSize_(0), taskSize_(0), idleThreadSize_(0), curThreadSize_(0),
          taskQueMaxThreshHold_(TASK_MAX_THRESHHOLD), threadSizeThreshHold_(THREAD_MAX_THRESHHOLD),
          poolMode_(PoolMode::MODE_FIXED), isPoolRunning_(false) {}


// 线程池析构
ThreadPool::~ThreadPool() {
//    isPoolRunning_ = false;
//
//    // 等待线程池里面所有的线程返回  有两种状态：阻塞 & 正在执行任务中
//    std::unique_lock<std::mutex> lock(taskQueMtx_);
//    notEmpty_.notify_all();
//    exitCond_.wait(lock, [&]()->bool {return threads_.size() == 0; });
}

// 设置线程池的工作模式
void ThreadPool::setMode(PoolMode mode) {
    if (checkRunningState())
        return;
    poolMode_ = mode;
}

// 设置task任务队列上线阈值
void ThreadPool::setTaskQueMaxThreshHold(int threshhold) {
    if (checkRunningState())
        return;
    taskQueMaxThreshHold_ = threshhold;
}

// 设置线程池cached模式下线程阈值
void ThreadPool::setThreadSizeThreshHold(int threshhold) {
    if (checkRunningState())
        return;
    if (poolMode_ == PoolMode::MODE_CACHED) {
        threadSizeThreshHold_ = threshhold;
    }
}

// 给线程池提交任务    用户调用该接口，传入任务对象，生产任务
void ThreadPool::submitTask(std::shared_ptr<Task> sp) {

}

// 开启线程池
void ThreadPool::start(int initThreadSize) {
    // 记录初始线程个数
    initThreadSize_ = initThreadSize;
    curThreadSize_ = initThreadSize;


    // 创建线程对象
    for (int i = 0; i < initThreadSize_; i++) {
        threads_.emplace_back(new Thread(std::bind(&ThreadPool::threadFunc, this)));
    }
    // 启动所有线程  std::vector<Thread*> threads_;
    for (int i = 0; i < initThreadSize_; i++) {
        threads_[i]->start(); // 需要去执行一个线程函数
        idleThreadSize_++;    // 记录初始空闲线程的数量
    }
}

// 定义线程函数   线程池的所有线程从任务队列里面消费任务
void ThreadPool::threadFunc()  // 线程函数返回，相应的线程也就结束了
{
    std::cout << "begin" << std::endl;
}

////////////////  线程方法实现
int Thread::generateId_ = 0;

// 线程构造
Thread::Thread(ThreadFunc func) {
    this->func_ = func;
}

Thread::~Thread() {}


// 启动线程
void Thread::start() {
    // 创建一个线程来执行一个线程函数 pthread_create
    std::thread t(func_);
    t.detach(); // 设置分离线程   pthread_detach  pthread_t设置成分离线程
}

int Thread::getId() const {
    return threadId_;
}

bool ThreadPool::checkRunningState() const
{
    return isPoolRunning_;
}