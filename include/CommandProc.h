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

#include <ThreadPool.h>
#include <command.h>
#include <utils.h>

namespace cmd {

template <std::size_t QueueCount, std::size_t NumberOfQueues>
class CommandProcessor final {
public:
  bool start();

  bool stop();

private:
  using CommadPipeline =
      std::array<std::array<Command, QueueCount>, NumberOfQueues>;

  std::mutex m_pipeLineMtx{};
  CommadPipeline m_pipeLine{};
};

} // namespace cmd
