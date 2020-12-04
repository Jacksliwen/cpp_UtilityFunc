#include <iostream>
#include <memory>
#include <string>
#include <vector>


// 根据网卡名获取本机ip
int GetLocalIp(const char *ethinf, char *ipaddr);

// base64编码
std::string Base64Encode(unsigned char const *bytes_to_encode,unsigned int in_len);

// base64解码
std::string Base64Decode(std::string const &encoded_string);

