#include "utils.hpp"

#include <chrono>

namespace triangulator {
namespace {
// Macros for disabling stdout.
#ifdef _WIN32
#include <io.h>
char * const nulFileName = "NUL";
#define CROSS_DUP(fd) _dup(fd)
#define CROSS_DUP2(fd, newfd) _dup2(fd, newfd)
#else
#include <unistd.h>
char * const nulFileName = "/dev/null";
#define CROSS_DUP(fd) dup(fd)
#define CROSS_DUP2(fd, newfd) dup2(fd, newfd)
#endif
  
// Functions for disabling stdout. Source: https://stackoverflow.com/questions/13498169/c-how-to-suppress-a-sub-functions-output.
int stdoutBackupFd; 
FILE *nullOut;
} // namespace

namespace utils {
void DisableStdout() {
  stdoutBackupFd = CROSS_DUP(STDOUT_FILENO);

  fflush(stdout);
  nullOut = fopen(nulFileName, "w");
  CROSS_DUP2(fileno(nullOut), STDOUT_FILENO);
}
void ReEnableStdout() {
  fflush(stdout);
  fclose(nullOut);
  CROSS_DUP2(stdoutBackupFd, STDOUT_FILENO);
  close(stdoutBackupFd);
}
} // namespace utils

int Log::log_level_ = 10000;
void Log::SetLogLevel(int lvl) {
  log_level_ = lvl;
}

Timer::Timer() {
  timing = false;
  elapsedTime = std::chrono::duration<double>(std::chrono::duration_values<double>::zero());
}

void Timer::start() {
  if (timing) return;
  timing = true;
  startTime = std::chrono::steady_clock::now();
}

void Timer::stop() {
  if (!timing) return;
  timing = false;
  std::chrono::time_point<std::chrono::steady_clock> endTime = std::chrono::steady_clock::now();
  elapsedTime += (endTime - startTime);
}

std::chrono::duration<double> Timer::getTime() {
  if (timing) {
    stop();
    std::chrono::duration<double> ret = elapsedTime;
    start();
    return ret;
  }
  else {
    return elapsedTime;
  }
}
} // namespace triangulator
