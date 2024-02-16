//
// Created by lhc456 on 2024/2/16.
//
#include "threadpool.h"

#include <functional>
#include <thread>
#include <iostream>

class MyTask : public Task {
public:
    MyTask();

    void run() {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "tid enter:" << std::this_thread::get_id() << std::endl;
    }
};

MyTask::MyTask() {

}

int main() {
    ThreadPool pool;
    // 开始启动线程池
    pool.start(4);
    pool.submitTask(std::make_shared<MyTask>());
    pool.submitTask(std::make_shared<MyTask>());
    pool.submitTask(std::make_shared<MyTask>());
    pool.submitTask(std::make_shared<MyTask>());
    pool.submitTask(std::make_shared<MyTask>());

    getchar();

}