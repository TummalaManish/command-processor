#pragma once

#include <array>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <future>
#include <mutex>
#include <thread>
#include <type_traits>

#include <command.h>
#include <utils.h>

namespace utils {

template <std::size_t NumberOfThreads> class ThreadPool final {
public: // Moved up so ExecutionPolicy is accessible for the template default
  enum class ExecutionPolicy : std::uint8_t { INSTANT, DELAYED };

  // Forward declarations and internal types
  class Err final {
  public:
    enum class Code : std::uint8_t {
      SUCCESS
      // ...
    };

    operator bool() const { return Code::SUCCESS == m_code; }

  private:
    Code m_code{Code::SUCCESS};
  };
  class InstantExecutor final {
  public:
    enum class Err : std::uint8_t {
      EXECUTION_COMPLETED,
      POOL_DEAD,
      // ...
    };

    Err launchJob(std::function<void(void)> &&);
    Err launchJob(const std::function<void(void)> &);
  };

  class DelayedExecutor final {
    enum class Err : std::uint8_t {
      EXECUTION_COMPLETED,
      POOL_DEAD,
      // ...
    };

    std::future<Err> launchJob(std::function<void(void)> &&);
    std::future<Err> launchJob(const std::function<void(void)> &);
  };

private:
  static constexpr bool isDelayedExecution(const ExecutionPolicy f_policy) {
    return ExecutionPolicy::DELAYED == f_policy;
  }

  class ThreadCntx final {
  public:
    enum class ThreadState : std::uint8_t {
      ACTIVE,
      INACTIVE,
      DEAD,
      NOT_INITIALIZED
    };

    std::future<void> move_job(std::function<void(void)> &&f_job) {
      unq_lk l_lck{m_mtx};
      if (ThreadState::INACTIVE == m_threadState) {
        m_packedJob = std::packaged_task<void(void)>{std::move(f_job)};
        m_threadState = ThreadState::ACTIVE;
        auto l_future = m_packedJob.get_future();
        l_lck.unlock();
        m_cv.notify_one();
        return l_future;
      }
      return {};
    }

    std::thread::native_handle_type getHandel() {
      return m_thread.native_handle();
    }

    ~ThreadCntx() {
      {
        lk_gd _{m_mtx};
        m_isReadyToTerminate = true;
      }
      m_cv.notify_all();

      if (m_thread.joinable()) {
        m_thread.join();
      }
    }

    ThreadCntx(const ThreadCntx &) = delete;
    ThreadCntx(ThreadCntx &&) = delete;
    ThreadCntx &operator=(const ThreadCntx &) = delete;
    ThreadCntx &operator=(ThreadCntx &&) = delete;

  private:
    /*
     *@breif Loop that thread internally runs.
     */
    void threadLoop() {
      unq_lk l_lck{m_mtx};
      m_threadState = ThreadState::INACTIVE;

      while (not m_isReadyToTerminate) {
        m_cv.wait(l_lck, [this]() {
          return (m_isReadyToTerminate || m_packedJob.valid());
        });

        if (not m_isReadyToTerminate) {
          l_lck.unlock();
          m_packedJob();
          l_lck.lock();
          m_threadState = ThreadState::INACTIVE;
        }
      }
      m_threadState = ThreadState::DEAD;
    }

    ThreadState m_threadState{ThreadState::NOT_INITIALIZED};
    mutable std::mutex m_mtx{};
    std::condition_variable m_cv{};
    bool m_isReadyToTerminate{};
    std::packaged_task<void(void)> m_packedJob{};
    std::thread m_thread{};
  };

  using CntxPool = std::array<ThreadCntx, NumberOfThreads>;
  CntxPool m_pool;

public:
  /*
   * @breif Returns an executor for executing jobs.
   */
  template <ExecutionPolicy Policy = ExecutionPolicy::DELAYED>
  auto getExecutor() ->
      typename std::conditional<Policy == ExecutionPolicy::INSTANT,
                                InstantExecutor, DelayedExecutor>::type;

  /*
   * @brief Waits for the threads to be initialized.
   */
  bool waitForInit(const std::chrono::milliseconds duration) { return false; }

  /*
   * @brief Waits for the threads to be initialized.
   */
  std::future<Err> waitForInit();
};

} // namespace utils
