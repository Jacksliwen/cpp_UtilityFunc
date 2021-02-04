#ifndef __QUEUE_BUFFER_H__
#define __QUEUE_BUFFER_H__

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

template <typename T>
class QueueBuffer {
 public:
  QueueBuffer(int max_size = 2048) : max_size_(max_size) {}

  void Put(std::shared_ptr<T> &new_value) {
    do {
      std::unique_lock<std::mutex> lck(mtx_);
      cond_put_.wait(lck, [this] { return max_size_ > data_.size(); });
      data_.push(std::move(new_value));
    } while (0);
    cond_get_.notify_one();
  }

  void Get(std::shared_ptr<T> &value) {
    do {
      std::unique_lock<std::mutex> lck(mtx_);
      cond_get_.wait(lck, [this]() { return !this->data_.empty(); });
      value = std::move(data_.front());
      data_.pop();
    } while (0);
    cond_put_.notify_one();
  }

  void TryPut(std::shared_ptr<T> &new_value,
              std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lck(mtx_);
    if (cond_put_.wait_for(lck, timeout,
                           [this] { return max_size_ > data_.size(); })) {
      data_.push(std::move(new_value));
      cond_get_.notify_one();
    }
  }

  void TryGet(std::shared_ptr<T> &value, std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lck(mtx_);
    if (cond_get_.wait_for(lck, timeout,
                           [this] { return !this->data_.empty(); })) {
      value = std::move(data_.front());
      data_.pop();
      cond_put_.notify_one();
    }
  }

 private:
  int max_size_;
  std::queue<std::shared_ptr<T>> data_;
  std::mutex mtx_;
  std::condition_variable cond_put_;
  std::condition_variable cond_get_;
};

#endif
