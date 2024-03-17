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
Result ThreadPool::submitTask(std::shared_ptr<Task> sp) {
    // 获取锁
    std::unique_lock<std::mutex> lock(taskQueMtx_);
    // 线程的通信  等待任务队列有空余   wait   wait_for   wait_until
    // 用户提交任务，最长不能阻塞超过1s，否则判断提交任务失败，返回
    if (!notFull_.wait_for(lock, std::chrono::seconds(1),
                           [&]() -> bool { return taskQue_.size() < (size_t) taskQueMaxThreshHold_; })) {
        // 表示notFull_等待1s种，条件依然没有满足
        std::cerr << "task queue is full, submit task fail." << std::endl;
        // return task->getResult();  // Task  Result   线程执行完task，task对象就被析构掉了
        return Result(sp, false);
    }

    // 线程的通信 等待
    notFull_.wait(lock, [&]() -> bool {
        return taskQue_.size() < taskQueMaxThreshHold_;
    });

    // 如果有空余的话 就放进去
    taskQue_.emplace(sp);
    taskSize_++;
    //因为新放进了任务，任务队列肯定不空，在notEmpty上面通知
    notEmpty_.notify_all();

    if (poolMode_ == PoolMode::MODE_CACHED
        && taskSize_ > idleThreadSize_
        && curThreadSize_ < threadSizeThreshHold_) {
        // 创建线程
        std::cout << ">>> create new thread..." << std::endl;
        auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this, std::placeholders::_1));
        int threadId = ptr->getId();
        threads_.emplace(threadId,std::move(ptr));
        threads_[threadId]->start();
        curThreadSize_++;
        idleThreadSize_++;
    }
    return Result(sp);
}

// 开启线程池
void ThreadPool::start(int initThreadSize) {
    isPoolRunning_ = true;

    // 记录初始线程个数
    initThreadSize_ = initThreadSize;
    curThreadSize_ = initThreadSize;


    // 创建线程对象
    for (int i = 0; i < initThreadSize_; i++) {
        // unique_ptr不允许使用左指引用，右值引用可以，通过std::move
        auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this, std::placeholders::_1));
//        threads_.emplace_back(std::move(ptr));
        int threadId = ptr->getId();
        threads_.emplace(threadId, std::move(ptr));

    }
    // 启动所有线程  std::vector<Thread*> threads_;
    for (int i = 0; i < initThreadSize_; i++) {
        threads_[i]->start(); // 需要去执行一个线程函数
        idleThreadSize_++;    // 记录初始空闲线程的数量
    }
}

// 定义线程函数   线程池的所有线程从任务队列里面消费任务
void ThreadPool::threadFunc(int threadid)  // 线程函数返回，相应的线程也就结束了
{
    auto lastTime = std::chrono::high_resolution_clock().now();

    // 所有任务必须执行完成，线程池才可以回收所有线程资源
    for (;;) {
        std::shared_ptr<Task> task;
        {
            // 先获取锁
            std::unique_lock<std::mutex> lock(taskQueMtx_);
            // 等待not empty
            // 等待notEmpty条件

            std::cout << "tid:" << std::this_thread::get_id()
                      << "尝试获取任务..." << std::endl;
            // cached 模式下，有可能已经创建了很多线程，但是空闲时间比较长
            // 需要对线程进行一定的回收
            if (poolMode_ == PoolMode::MODE_CACHED) {
                if (std::cv_status::timeout ==
                    notEmpty_.wait_for(lock, std::chrono::seconds(1))) {
                    auto now = std::chrono::high_resolution_clock().now();
                    auto dur = std::chrono::duration_cast<std::chrono::seconds>(now - lastTime);
                    if (dur.count() >= THREAD_MAX_IDLE_TIME
                        && curThreadSize_ > initThreadSize_) {
                        // 开始回收线程
                        //  threads_.erase(threadid); // std::this_thread::getid()
                        threads_.erase(threadid); // std::this_thread::getid()

                        curThreadSize_--;
                        idleThreadSize_--;

                        std::cout << "threadid:" << std::this_thread::get_id() << " exit!"
                                  << std::endl;
                        return;
                    }

                };

            } else {

                notEmpty_.wait(lock, [&]() -> bool {
                    return taskQue_.size() > 0;
                });
            }

            idleThreadSize_--;
            // 从任务队列种取一个任务出来
            task = taskQue_.front();
            taskQue_.pop();
            taskSize_--;
            std::cout << "tid:" << std::this_thread::get_id()
                      << "获取任务成功..." << std::endl;


            // 如果依然有剩余任务，继续通知其它得线程执行任务
            if (taskQue_.size() > 0) {
                notEmpty_.notify_all();
            }
            // 取出一个任务，进行通知，通知可以继续提交生产任务
            notFull_.notify_all();

            // cached模式 任务处理比较紧急 场景：少而块 需要根据任务数量和空闲的数量，判断是否需要创建新的线程
        }     // 就应该把锁释放掉
        // 当前线程负责执行这个任务
        if (task != nullptr) {
//            task->run(); // 执行任务；把任务的返回值setVal方法给到Result
            task->exec();
        }
        idleThreadSize_++;
        lastTime = std::chrono::high_resolution_clock().now(); // 更新最后的时间

    }

}

////////////////  线程方法实现
int Thread::generateId_ = 0;

// 线程构造
Thread::Thread(ThreadFunc func)
        : func_(func), threadId_(generateId_++) {}

Thread::~Thread() {}


// 启动线程
void Thread::start() {
    // 创建一个线程来执行一个线程函数 pthread_create
    std::thread t(func_, threadId_);
    t.detach(); // 设置分离线程   pthread_detach  pthread_t设置成分离线程
}

int Thread::getId() const {
    return threadId_;
}

bool ThreadPool::checkRunningState() const {
    return isPoolRunning_;
}


/////////////////   Result方法的实现
Result::Result(std::shared_ptr<Task> task, bool isValid)
        : isValid_(isValid), task_(task) {
    task_->setResult(this);
}


Any Result::get() // 用户调用的
{
    if (!isValid_) {
        return "";
    }
    sem_.wait(); // task任务如果没有执行完，这里会阻塞用户的线程
    return std::move(any_);
}


void Result::setVal(Any any)  // 谁调用的呢？？？
{
    // 存储task的返回值
    this->any_ = std::move(any);
    sem_.post(); // 已经获取的任务的返回值，增加信号量资源
}


/////////////////  Task方法实现
Task::Task()
        : result_(nullptr) {}

void Task::exec() {
    if (result_ != nullptr) {
        result_->setVal(run()); // 这里发生多态调用
    }
}

void Task::setResult(Result *res) {
    result_ = res;
}