#include <sys/stat.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace cpp_utilit {

// 根据网卡名获取本机ip
std::string GetLocalIp(const char* ethinf);

// 获取程序运行路径,以及进程名
size_t GetModulePath(char* processdir, char* processname, size_t len);

// base64编编码相关----------
std::string Base64Encode(unsigned char const* bytes_to_encode,
                         unsigned int in_len);
std::string Base64Decode(std::string const& encoded_string);

// 时间相关------------------
// 时间戳(秒)转YYYY-MM-DDTHH:mm:SS.000Z
std::string TimeT2LocalString(time_t nTime);

// 时间戳(秒)转UTC YYYY-MM-DDTHH:mm:SS.000Z
std::string TimeT2UTCString(time_t nTime);

// 时间戳转UTC时间戳
time_t TimeT2UTCTimeT(time_t nTime);

// 获取timeBefore之前的日期   20201205
std::string GetLocalDate(int timeBefore = 0);

// 根据时间戳获取日期    20201205
std::string GetDateFromTimestamp(time_t time);

// 计算end -start花费时间
double CalculationTimeCosted(std::chrono::system_clock::time_point start,
                             std::chrono::system_clock::time_point end);

// 系统信号处理相关-------------
// 程序收到非core信号，不直接退出，指定退出函数
bool initSignal();

// 文件是否存在
bool FileExist(const std::string& name);

// 读取图片二进制数据
char* ReadImage(std::string picPath, int& length);

// 判断图片类型
std::string CheckImageType(unsigned char* imagebuf);

}  // namespace cpp_utilit
