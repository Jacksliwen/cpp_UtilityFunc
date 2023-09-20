#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <thread>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>
#include <atomic>


inline int getTerminalWidth() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return w.ws_col;
}

class ProgressBar {
      int max_num_,index_;
      std::string bar_name_;
public:
    ProgressBar(int max_num) : index_(0),bar_name_(""), max_num_(max_num) {}
    ProgressBar(std::string bar_name, int max_num) : index_(0),bar_name_(bar_name), max_num_(max_num) {}

    void increment(int value=1) {
        index_ += value;
        if (index_ > max_num_) {
            index_=max_num_;
        }
    }

    void set_schedule(int schedule) {
        index_ = schedule;
        if (index_ > max_num_) {
            index_=max_num_;
        }
    }

    std::string toString() {
        auto scheduleLength = getTerminalWidth() - 11 - bar_name_.size() ;
        float a = static_cast<float>(index_) / static_cast<float>(max_num_);
        int pa = static_cast<int>(a * scheduleLength);

        if (pa > scheduleLength) {
            pa = scheduleLength;
        }
        std::stringstream ss;
        ss << bar_name_ <<":[" + std::string(pa, '#') + std::string(scheduleLength - pa, ' ') << "]"
           << " " << std::setiosflags(std::ios::fixed) << std::setprecision(2) << a * 100 << "%" << std::endl;
        return ss.str();
    }
};

class ProgressBars
{
public:
    void addBar(ProgressBar* bar) {
        bars.push_back(bar);
    }

    void print() {
        for (int i = 0; i < bars.size(); i++) {
            std::cout << "\33[2K";
        }
        std::cout << "\033[" << bars.size() << "A\r";
        for (auto& bar : bars) {
            std::cout << bar->toString();
        }

        std::cout.flush();
    }
private:
    std::vector<ProgressBar*> bars;
};

int main() {
    ProgressBar bar1("test1",100), bar2("bar222",200), bar3("bar3", 300),bar4(100);
    ProgressBars bars;
    bars.addBar(&bar1);
    bars.addBar(&bar2);
    bars.addBar(&bar3);
    bars.addBar(&bar4);

    std::vector<std::thread> ts;
    std::atomic<uint8_t> finish_state;
    finish_state.store(0);

    ts.emplace_back([&bar1,&finsh_state](){
      for (int i = 0; i < 100; i++) {
          if (i < 100) bar1.increment();
          std::this_thread::sleep_for(std::chrono::milliseconds(20));
      }
      finish_state++;
    });

    ts.emplace_back([&bar2,&finsh_state](){
      for (int i = 0; i < 200; i++) {
          bar2.increment();
          std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }
      finish_state++;
    });
    ts.emplace_back([&bar3,&finsh_state](){
      for (int i = 0; i < 300; i++) {
          bar3.increment();
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
      finish_state++;
    });

    ts.emplace_back([&bar4,&finsh_state](){
      for (int i = 0; i < 50; i++) {
          bar4.increment(2);
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
      bar4.set_schedule(65);
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      bar4.set_schedule(70);
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      bar4.set_schedule(80);
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      bar4.set_schedule(85);
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      bar4.set_schedule(100);
      finish_state++;
    });

    for (;finish_state < ts.size();) {
        bars.print();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    for(auto i=0; i< ts.size(); i++){
      if(ts[i].joinable()) ts[i].join();
    }
    return 0;
}
