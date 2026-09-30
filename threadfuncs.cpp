#include "threadfuncs.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <condition_variable>

// ЗАДАНИЕ 20: глобальный счётчик
std::atomic<int> counter{0};

// ---------------------------------------------------------------------------
// Logger
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// Вспомогательные
// ---------------------------------------------------------------------------
pid_t getThreadID() {
  return static_cast<pid_t>(::syscall(SYS_gettid));
}

void about() {
  std::cout << "std::thread example\n";
}

// ---------------------------------------------------------------------------
// Блок 7: обычный поток
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

  // ЗАДАНИЕ 20: 100000 инкрементов — один раз на поток (вне внешнего цикла)
  for (int k = 0; k < 100000; ++k) {
    counter++;
  }
}

// ---------------------------------------------------------------------------
// Блок 9, задание 14/16: поток с promise
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
// Блок 9, задание 16: поток для std::async
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

// ---------------------------------------------------------------------------
// Блок 10, задание 21: производитель-потребитель
// ---------------------------------------------------------------------------
namespace {

std::mutex                pc_mutex;
std::condition_variable   pc_cv;

int  shared_value = 0;
bool data_ready   = false;
bool done         = false;

constexpr int TRANSFERS = 10;

} // namespace

void producer(Logger& logger) {
  for (int i = 1; i <= TRANSFERS; ++i) {
    {
      std::lock_guard<std::mutex> lock(pc_mutex);
      shared_value = i;
      data_ready   = true;
    }
    pc_cv.notify_one();

    // используем logger — пишем в лог из производителя
    {
      std::ostringstream oss;
      oss << "producer: put value = " << i;
      logger.writeLine(oss.str());
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  {
    std::lock_guard<std::mutex> lock(pc_mutex);
    done = true;
  }
  pc_cv.notify_one();

  logger.writeLine("producer: done, exiting");
}

void consumer(Logger& logger) {
  while (true) {
    std::unique_lock<std::mutex> lock(pc_mutex);
    pc_cv.wait(lock, [] { return data_ready || done; });

    if (data_ready) {
      int value = shared_value;
      data_ready = false;
      lock.unlock();

      std::ostringstream oss;
      oss << "consumer: got value = " << value;
      logger.writeLine(oss.str());
    } else if (done) {
      break;
    }
  }
  logger.writeLine("consumer: done, exiting");
}
