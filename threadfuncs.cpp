#include "threadfuncs.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>

Logger::Logger(const std::string& filename)
  : file_(filename, std::ios::out | std::ios::trunc)
{
  if (!file_.is_open()) {
    throw std::runtime_error("Cannot open log file: " + filename);
  }
}

Logger::~Logger() {
  // std::ofstream closes the file automatically
}

// Задание 6: возвращаем bool — true при успешной записи
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

void funcThread(const ThreadArgs& args, Logger& logger) {
  for (int i = 0; i < COUNT_ITERATIONS; ++i) {
    std::ostringstream oss;

    // Задание 12: добавляем std::this_thread::get_id()
    oss << "[tag = " << args.tag
        << "] std::thread::id = " << std::this_thread::get_id()
        << " sys tid = "         << getThreadID()
        << " pid = "             << ::getpid()
        << " ppid = "            << ::getppid()
        << " iter = "            << i;

    // Задание 8: пишем через logger, результат не проверяем пока
    logger.writeLine(oss.str());

    // imitation of useful work
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
