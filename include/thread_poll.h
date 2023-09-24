#ifndef __THREAD_POLL_H__
#define __THREAD_POLL_H__

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

class ThreadPool {
public:
    using Task = std::function<void()>;

    explicit ThreadPool(size_t numThreads) : stop(false) {
        for (size_t i = 0; i < numThreads; ++i)
            workers.emplace_back(
                [this] {
                    while (true) {
                        Task task;
                        {
                            std::unique_lock<std::mutex> lock(queueMutex);
                            condition.wait(lock, [this] {
                                return stop || !tasks.empty();
                            });
                            if (stop && tasks.empty())
                                return;
                            task = std::move(tasks.front());
                            tasks.pop();
                        }
                        task();
                    }
                }
            );
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker : workers)
            worker.join();
    }

    template<class F>
    void enqueue(F&& task) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.emplace(std::forward<F>(task));
        }
        condition.notify_one();
    }

private:
    std::vector<std::thread> workers;
    std::queue<Task> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};

int main() {
    // 创建线程池，指定线程数量为4
    ThreadPool pool(4);

    // 向线程池添加任务（这里示意添加了10个任务）
    for (int i = 0; i < 10; ++i) {
        pool.enqueue([i] {
            std::cout << "Task " << i << " executed." << std::endl;
        });
    }

    // 等待所有任务执行完毕，析构函数会自动销毁线程池
    std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}

#endif
