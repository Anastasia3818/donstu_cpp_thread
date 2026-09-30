#include "threadfuncs.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>

// ЗАДАНИЕ 20
std::atomic<int> counter{0};

Logger::Logger(const std::string& filename)
  : file_(filename, std::ios::out | std::ios::trunc)
{
  if (!file_.is_open()) {
    throw std::runtime_error("Cannot open log file: " + filename);
  }
}

Logger::~Logger() {}

bool Logger::writeLine(const std::string& msg) {
  std::lock_guard<std::mutex> lock(mutex_);
  file_ << msg;
  file_.flush();
  return static_cast<bool>(file_);
}

pid_t getThreadID() {
  return static_cast<pid_t>(::syscall(SYS_gettid));
}

void about() {
  std::cout << "std::thread example\n";
}

// ---------------------------------------------------------------------------
// Обычный поток
// ---------------------------------------------------------------------------
void funcThread(const ThreadArgs& args, Logger& logger) {
  for (int i = 0; i < COUNT_ITERATIONS; ++i) {
    std::ostringstream oss;

    oss << "[tag = " << args.tag
        << "] std::thread::id = " << std::this_thread::get_id()
        << " sys tid = " << getThreadID()
        << " pid = " << ::getpid()
        << " ppid = " << ::getppid()
        << " iter = " << i
        << "\n";

    logger.writeLine(oss.str());

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  // ЗАДАНИЕ 20: 100000 инкрементов — один раз на поток (вне цикла)
  for (int k = 0; k < 100000; ++k) {
    counter++;
  }
}

// ---------------------------------------------------------------------------
// ЗАДАНИЕ 14/16: поток с promise
// ---------------------------------------------------------------------------
void funcThreadWithResult(const ThreadArgs& args,
                          Logger& logger,
                          std::promise<std::string> prom) {
  int iterations_done = 0;

  for (int i = 0; i < COUNT_ITERATIONS; ++i) {
    std::ostringstream oss;

    oss << "[tag = " << args.tag
        << "] std::thread::id = " << std::this_thread::get_id()
        << " sys tid = " << getThreadID()
        << " iter = " << i
        << " (with result)\n";

    logger.writeLine(oss.str());
    ++iterations_done;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  std::ostringstream res;
  res << "thread " << args.tag
      << " done, iterations = " << iterations_done;
  prom.set_value(res.str());
}

// ---------------------------------------------------------------------------
// ЗАДАНИЕ 16: вариант для std::async
// ---------------------------------------------------------------------------
std::string funcThreadReturning(const ThreadArgs& args, Logger& logger) {
  int iterations_done = 0;

  for (int i = 0; i < COUNT_ITERATIONS; ++i) {
    std::ostringstream oss;

    oss << "[tag = " << args.tag
        << "] std::thread::id = " << std::this_thread::get_id()
        << " sys tid = " << getThreadID()
        << " (async)\n";

    logger.writeLine(oss.str());
    ++iterations_done;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  std::ostringstream res;
  res << "async " << args.tag
      << " done, iterations = " << iterations_done;
  return res.str();
}
