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
#include <condition_variable>

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

// обычная функция потока (блок 7)
void funcThread(const ThreadArgs& args, Logger& logger);

// ЗАДАНИЕ 14/16: поток с promise (блок 9)
void funcThreadWithResult(const ThreadArgs& args,
                          Logger& logger,
                          std::promise<std::string> prom);

// ЗАДАНИЕ 16: поток для std::async (блок 9)
std::string funcThreadReturning(const ThreadArgs& args, Logger& logger);

// ЗАДАНИЕ 21: производитель-потребитель (блок 10)
void producer(Logger& logger);
void consumer(Logger& logger);

pid_t getThreadID();
void about();
