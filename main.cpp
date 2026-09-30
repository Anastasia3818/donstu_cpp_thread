#include <iostream>
#include <vector>
#include <thread>
#include <sstream>
#include <functional>
#include <future>

#include "threadfuncs.h"

int main() {
  about();

  Logger logger("output.log");

  {
    std::ostringstream oss;
    oss << "main: pid = " << getThreadID()
        << ", opened file: 'output.log'";
    logger.writeLine(oss.str());
  }

  // ---- args ----
  std::vector<ThreadArgs> args(COUNT_THREADS);
  for (int i = 0; i < COUNT_THREADS; ++i) {
    std::ostringstream oss;
    oss << "T" << i;
    args[i].id  = i;
    args[i].tag = oss.str();
  }

  // ---- обычные потоки ----
  std::vector<std::thread> threads;
  threads.reserve(COUNT_THREADS);

  for (int i = 0; i < COUNT_THREADS; ++i) {
    threads.emplace_back(funcThread, std::cref(args[i]), std::ref(logger));
  }

  for (auto& t : threads) {
    if (t.joinable()) t.join();
  }

  std::cout << "counter = " << counter
            << " (expected " << COUNT_THREADS * 100000 << ")\n";

  logger.writeLine("main: all threads finished, file closed");

  // ---- ЗАДАНИЕ 14/16: promise/future ----
  std::cout << "\n--- Блок 9: promise/future ---\n";

  std::promise<std::string> prom;
  std::future<std::string>  fut = prom.get_future();

  std::thread tWithResult(funcThreadWithResult,
                          std::cref(args[0]),
                          std::ref(logger),
                          std::move(prom));

  std::string result = fut.get();
  std::cout << "got from thread (promise): " << result << "\n";

  tWithResult.join();

  // ---- ЗАДАНИЕ 16: std::async ----
  std::cout << "\n--- Блок 9: std::async ---\n";

  std::future<std::string> fut2 = std::async(std::launch::async,
                                             funcThreadReturning,
                                             std::cref(args[1]),
                                             std::ref(logger));

  std::string result2 = fut2.get();
  std::cout << "got from thread (async): " << result2 << "\n";

  logger.writeLine("main: block 9 done");
  return 0;
}
