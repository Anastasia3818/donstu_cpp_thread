#pragma once

#include <thread>
#include <chrono>
#include <sstream>
#include <iostream>
#include <string>
#include <mutex>
#include <fstream>
#include <atomic>
#include <future>

#include <sys/types.h>

// count of threads and iterations
inline constexpr int COUNT_THREADS    = 4;
inline constexpr int COUNT_ITERATIONS = 3;

// ЗАДАНИЕ 20: глобальный счётчик
extern std::atomic<int> counter;

struct ThreadArgs {
  int         id;
  std::string tag;
};

class Logger {
public:
  explicit Logger(const std::string& filename);
  ~Logger();

  bool writeLine(const std::string& msg);

  Logger(const Logger&)            = delete;
  Logger& operator=(const Logger&) = delete;

private:
  std::ofstream    file_;
  std::mutex       mutex_;
};

// обычная функция потока
void funcThread(const ThreadArgs& args, Logger& logger);

// ЗАДАНИЕ 14/16: поток с promise
void funcThreadWithResult(const ThreadArgs& args,
                          Logger& logger,
                          std::promise<std::string> prom);

// ЗАДАНИЕ 16: поток для std::async
std::string funcThreadReturning(const ThreadArgs& args, Logger& logger);

pid_t getThreadID();
void about();
