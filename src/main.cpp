#include <unistd.h>

#include <iostream>

#include "func.h"
#include "process_bar.h"

using namespace cpp_utilit;

int main(int argc, char** argv) {
  initSignal();

  std::cout << "ip: " << GetLocalIp("em1") << std::endl;

  ProgressBars_Test();

  //--------------------
#define MAX_PATH 260
  char szFileName[MAX_PATH] = {0};
  char szProcessName[MAX_PATH] = {0};
  GetModulePath(szFileName, szProcessName, MAX_PATH);
  std::cout << szFileName << ",\n" << szProcessName << std::endl;

  //---------------------

  auto start = std::chrono::system_clock::now();
  // do something...
  usleep(200000);
  usleep(300000);
  auto end = std::chrono::system_clock::now();

  std::cout << CalculationTimeCosted(start, end) << std::endl;
  //--------------------

  //   while (getchar() != 'q') {
  //     usleep(500000);
  //   }
  return 0;
}
