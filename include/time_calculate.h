#ifndef __TIME_CALCULATE_H__
#define __TIME_CALCULATE_H__

#include <chrono>
#include <functional>
#include <iostream>
#include <vector>

namespace cpp_utilit {
class ElapsedTime {
  // nanoseconds microseconds milliseconds seconds minutes hours
 public:
  ElapsedTime() { start_time_ = std::chrono::system_clock::now(); }

  ~ElapsedTime() {
    ResetTime();
    PrintAll();
  }


  void ResetTime(std::string name = "") {
    if (name.empty()) name = std::string("index_") + std::to_string(time_index_);
    end_time_ = std::chrono::high_resolution_clock::now();
    time_list_.emplace_back(std::pair(name, Calculation()));
    start_time_ = end_time_;
  }

  void PrintAll(){
    for (auto &iter: time_list_)
        std::cout << iter.first << " use time: " << std::to_string(iter.second) << std::endl;    
  }

  void run(std::function<void()> func) {
    start_time_ = std::chrono::high_resolution_clock::now();
    func();
    end_time_ = std::chrono::high_resolution_clock::now();
  }

private:
  double Calculation() {
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        end_time_ - start_time_);
    return double(duration.count()) * std::chrono::microseconds::period::num /
           std::chrono::microseconds::period::den;
  }

 private:
  static uint16_t time_index_;
  std::chrono::system_clock::time_point start_time_;
  std::chrono::system_clock::time_point end_time_;
  std::vector<std::pair<std::string, double>> time_list_;
};

}  // namespace cpp_utilit

#endif
