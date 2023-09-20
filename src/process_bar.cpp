#include "process_bar.h"

namespace cpp_utilit {
std::string ProgressBar::ToString() {
  auto scheduleLength = getTerminalWidth() - 11 - bar_name_.size();
  float a = static_cast<float>(index_) / static_cast<float>(max_num_);
  int pa = static_cast<int>(a * scheduleLength);

  if (pa > scheduleLength) {
    pa = scheduleLength;
  }
  std::stringstream ss;
  ss << bar_name_
     << ":[" + std::string(pa, '#') + std::string(scheduleLength - pa, ' ')
     << "]"
     << " " << std::setiosflags(std::ios::fixed) << std::setprecision(2)
     << a * 100 << "%" << std::endl;
  return ss.str();
}

void ProgressBar::Increment(int value) {
  index_ += value;
  if (index_ > max_num_) {
    index_ = max_num_;
    is_finished_.store(true);
  }
}

void ProgressBar::SetSchedule(int schedule) {
  index_ = schedule;
  if (index_ > max_num_) {
    index_.store(max_num_);
    is_finished_.store(true);
  }
}

void ProgressBars::AddBar(ProgressBar* bar) { bars.push_back(bar); }

void ProgressBars::Print() {
  for (int i = 0; i < bars.size(); i++) {
    std::cout << "\33[2K";
  }
  std::cout << "\033[" << bars.size() << "A\r";

  for (auto& bar : bars) {
    std::cout << bar->ToString();
  }
  std::cout.flush();
}

int ProgressBars_Test() {
  ProgressBar bar1("test1", 100), bar2("bar222", 200), bar3("bar3", 300),
      bar4(100);
  ProgressBars bars;
  bars.AddBar(&bar1);
  bars.AddBar(&bar2);
  bars.AddBar(&bar3);
  bars.AddBar(&bar4);

  std::vector<std::thread> ts;
  std::atomic<uint8_t> finish_state;
  finish_state.store(0);

  ts.emplace_back([&bar1, &finish_state]() {
    for (int i = 0; i < 100; i++) {
      if (i < 100) bar1.Increment();
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    finish_state++;
  });

  ts.emplace_back([&bar2, &finish_state]() {
    for (int i = 0; i < 200; i++) {
      bar2.Increment();
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    finish_state++;
  });

  ts.emplace_back([&bar3, &finish_state]() {
    for (int i = 0; i < 300; i++) {
      bar3.Increment();
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    finish_state++;
  });

  ts.emplace_back([&bar4, &finish_state]() {
    for (int i = 0; i < 50; i++) {
      bar4.Increment(2);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    bar4.SetSchedule(65);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    bar4.SetSchedule(70);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    bar4.SetSchedule(80);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    bar4.SetSchedule(85);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    bar4.SetSchedule(100);
    finish_state++;
  });

  for (; finish_state < ts.size();) {
    bars.Print();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    // return 0;
  }

  for (auto i = 0; i < ts.size(); i++) {
    if (ts[i].joinable()) ts[i].join();
  }
  return 0;
}

}  // namespace cpp_utilit
