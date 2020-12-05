#include "func.h"

#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <algorithm>
#include <functional>

inline void terminate(int signal, siginfo_t *info, void *data) {
  std::cout << "shutdown recv signal[" << signal << "]" << std::endl;
  struct sigaction action;
  sigemptyset(&action.sa_mask);
  action.sa_handler = SIG_DFL;

  sigaction(signal, &action, 0);
  raise(signal);
}

bool initSignal() {
  struct sigaction action;
  sigemptyset(&action.sa_mask);

  action.sa_sigaction = terminate;
  action.sa_flags |= SA_SIGINFO;

  if (sigaction(SIGTERM, &action, NULL) < 0 ||
      sigaction(SIGINT, &action, NULL) < 0 ||
      sigaction(SIGFPE, &action, NULL) < 0 ||
      sigaction(SIGABRT, &action, NULL) < 0 ||
      sigaction(SIGSEGV, &action, NULL) < 0) {
    return false;
  }
  return true;
}

size_t GetModulePath(char *processdir, char *processname, size_t len) {
  char *path_end;
  if (readlink("/proc/self/exe", processdir, len) <= 0) return -1;
  path_end = strrchr(processdir, '/');
  if (path_end == NULL) return -1;
  ++path_end;
  strcpy(processname, path_end);
  *path_end = '\0';
  return (size_t)(path_end - processdir);
}

std::string GetLocalIp(const char *ethinf) {
  char ipaddr[32];
  struct sockaddr_in sin;
  struct ifreq ifr;
  do {
    int sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == sd) {
      printf("[%d:%d]socket error: %s", __FUNCTION__, __LINE__,
             strerror(errno));
      break;
    }
    strncpy(ifr.ifr_name, ethinf, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;

    // if error: No such device
    if (ioctl(sd, SIOCGIFADDR, &ifr) < 0) {
      printf("[%d:%d]ioctl error:%s", __FUNCTION__, __LINE__, strerror(errno));
      close(sd);
      break;
    }
    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
    snprintf(ipaddr, 32, "%s", inet_ntoa(sin.sin_addr));
    close(sd);
  } while (0);
  return std::string(ipaddr);
}

static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string Base64Encode(unsigned char const *bytes_to_encode,
                         unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] =
          ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] =
          ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for (i = 0; (i < 4); i++) {
        ret += base64_chars[char_array_4[i]];
      }
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 3; j++) char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] =
        ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] =
        ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++) {
      ret += base64_chars[char_array_4[j]];
    }

    while ((i++ < 3)) ret += '=';
  }

  return ret;
}

std::string Base64Decode(std::string const &encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;
  ret.reserve(encoded_string.length());

  while (in_len-- && (encoded_string[in_] != '=') &&
         is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_];
    in_++;
    if (i == 4) {
      for (i = 0; i < 4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] =
          (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] =
          ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++) {
        ret += char_array_3[i];
      }
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 4; j++) {
      char_array_4[j] = 0;
    }

    for (j = 0; j < 4; j++) {
      char_array_4[j] = base64_chars.find(char_array_4[j]);
    }

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] =
        ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) {
      ret += char_array_3[j];
    }
  }

  return ret;
}

std::string TimeT2LocalString(time_t nTime) {
  struct tm tm1;
  localtime_r((const time_t *)&nTime, &tm1);
  char szGatherTime[64];
  sprintf(szGatherTime, "%04d-%02d-%02dT%02d:%02d:%02d.000Z",
          tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday, tm1.tm_hour,
          tm1.tm_min, tm1.tm_sec);
  return szGatherTime;
}

std::string TimeT2UTCString(time_t nTime) {
  struct tm *tm1;
  tm1 = gmtime(&nTime);

  char szGatherTime[64];
  sprintf(szGatherTime, "%04d-%02d-%02dT%02d:%02d:%02d.000Z",
          tm1->tm_year + 1900, tm1->tm_mon + 1, tm1->tm_mday, tm1->tm_hour,
          tm1->tm_min, tm1->tm_sec);
  return szGatherTime;
}

time_t TimeT2UTCTimeT(time_t nTime) {
  struct tm *tm1;
  tm1 = gmtime(&nTime);
  return mktime(tm1);
}

std::string GetLocalDate(int timeBefore) {
  time_t now_time = time(0) - timeBefore;
  struct tm *local_now_time = localtime(&now_time);
  char ch[256];
  sprintf(ch, "%d%02d%02d", local_now_time->tm_year + 1900,
          local_now_time->tm_mon + 1, local_now_time->tm_mday);
  return std::string(ch);
}

std::string GetDateFromTimestamp(time_t time) {
  struct tm *local_now_time = localtime(&time);
  char ch[256];
  sprintf(ch, "%d%02d%02d", local_now_time->tm_year + 1900,
          local_now_time->tm_mon + 1, local_now_time->tm_mday);
  return std::string(ch);
}

double CalculationTimeCosted(std::chrono::system_clock::time_point start,
                             std::chrono::system_clock::time_point end) {
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  return double(duration.count()) * std::chrono::microseconds::period::num /
         std::chrono::microseconds::period::den;
}