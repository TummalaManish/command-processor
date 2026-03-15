#pragma once

#include <array>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <expected>
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
    friend ThreadPool;

    enum class ThreadState : std::uint8_t {
      ACTIVE,
      INACTIVE,
      DEAD,
      NOT_INITIALIZED
    };

    /*
     * @breif Moves the job to execution.
     */
    bool move_job(std::function<void(void) &&>);

    // TODO: This might have internal consistey problems.
    ThreadState getThreadState() const {
      ThreadState l_threadState{ThreadState::NOT_INITIALIZED};
      {
        lk_gd _{m_mtx};
        l_threadState = m_threadState;
      }
      return l_threadState;
    }

    std::thread::native_handle_type getHandel() {
      return m_thread.native_handle();
    }

    ~ThreadCntx();

  private:
    /*
     * @breif Loop that thread internally runs.
     */
    void threadLoop();

    ThreadState m_threadState{ThreadState::NOT_INITIALIZED};
    mutable std::mutex m_mtx{};
    std::condition_variable m_cv{};
    std::function<void(void)> m_job{};
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
  bool waitForInit(const std::chrono::milliseconds duration);

  /*
   * @brief Waits for the threads to be initialized.
   */
  std::future<Err> waitForInit();
};

} // namespace utils
