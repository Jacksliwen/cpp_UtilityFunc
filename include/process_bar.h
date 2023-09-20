#include <sys/ioctl.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <sstream>
namespace cpp_utilit {

int ProgressBars_Test();
class ProgressBar {
 public:
  ProgressBar(int max_num): index_(0),is_finished_(false),bar_name_(""), max_num_(max_num) {}
  ProgressBar(std::string bar_name, int max_num)
      : index_(0),is_finished_(false),bar_name_(bar_name), max_num_(max_num) {}

  void Increment(int value = 1);
  void SetSchedule(int schedule);

  inline int GetSchedule(){return index_;}
  inline bool IsFinished(){return is_finished_;}
  std::string ToString();

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
  void AddBar(ProgressBar* bar);

  void Print();

  // try to test
  int Test();

 private:
  std::vector<ProgressBar*> bars;
};
}  // namespace cpp_utilit
