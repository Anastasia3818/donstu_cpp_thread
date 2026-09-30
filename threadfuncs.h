#pragma once

#include <thread>
#include <chrono>
#include <sstream>
#include <iostream>
#include <string>
#include <mutex>
#include <fstream>

#include <sys/types.h>   // pid_t

// count of threads and iterations
inline constexpr int COUNT_THREADS    = 4;
inline constexpr int COUNT_ITERATIONS = 3;

// args for thread
struct ThreadArgs {
  int         id;
  std::string tag;
};

// common resources in separate class
class Logger {
public:
  explicit Logger(const std::string& filename);
  ~Logger();

  // write line with mutex; returns true on success
  bool writeLine(const std::string& msg);

  // block copy and move
  Logger(const Logger&)            = delete;
  Logger& operator=(const Logger&) = delete;

private:
  std::ofstream    file_;
  std::mutex       mutex_;
};

// function for thread
void funcThread(const ThreadArgs& args, Logger& logger);

// get system TID for current linux thread
pid_t getThreadID();

// headline of software
void about();
