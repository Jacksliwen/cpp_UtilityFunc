#ifndef __PROCESS_BAR_H__
#define __PROCESS_BAR_H__

#include <sys/ioctl.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
namespace cpp_utilit {

int ProgressBars_Test();
class ProgressBar {
 public:
  ProgressBar(int max_num)
      : index_(0), is_finished_(false), bar_name_(""), max_num_(max_num) {}
  ProgressBar(std::string bar_name, int max_num)
      : index_(0),
        is_finished_(false),
        bar_name_(bar_name),
        max_num_(max_num) {}

  void increase(int value = 1);
  void decrease(int value = 1);

  inline int GetSchedule() { return index_; }
  inline void SetSchedule(int schedule) {
    index_ = schedule;
    if (index_ > max_num_) {
      index_.store(max_num_);
      is_finished_.store(true);
    }
  }

  inline bool IsFinished() { return is_finished_; }
  std::string ToString();

  // struct winsize {
  //     int ws_row;    // 屏幕行数
  //     int ws_col;    // 屏幕列数
  //     int ws_xpixel; // 屏幕宽度像素数
  //     int ws_ypixel; // 屏幕高度像素数
  // };
  inline int getTerminalWidth() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
  }

 private:
  int max_num_;
  std::atomic<int> index_;
  std::atomic<bool> is_finished_;
  std::string bar_name_;
};

class ProgressBars {
 public:
  void AddBar(ProgressBar *bar);

  void Print();

 private:
  std::vector<ProgressBar *> bars;
};
}  // namespace cpp_utilit

#endif