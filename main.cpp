#include <iostream>
#include <vector>
#include <thread>
#include <sstream>
#include <functional>

#include "threadfuncs.h"

int main() {
  about();

  // Open log file
  Logger logger("output.log");

  // Задание 8: пишем в лог через logger.writeLine
  {
    std::ostringstream oss;
    oss << "main: pid = " << getThreadID()
        << ", opened file: 'output.log'";
    logger.writeLine(oss.str());
  }

  // Задание 7: формируем теги циклом T0..T3 вместо ручного списка
  std::vector<ThreadArgs> args(COUNT_THREADS);
  for (int i = 0; i < COUNT_THREADS; ++i) {
    std::ostringstream oss;
    oss << "T" << i;
    args[i].id  = i;
    args[i].tag = oss.str();
  }

  // threads are starting
  std::vector<std::thread> threads;
  threads.reserve(COUNT_THREADS);

  for (int i = 0; i < COUNT_THREADS; ++i) {
    threads.emplace_back(funcThread, std::cref(args[i]), std::ref(logger));
  }

  // wait for all threads to finish
  for (auto& t : threads) {
    if (t.joinable()) t.join();
  }

  logger.writeLine("main: all threads finished, file closed");
  return 0;
}
