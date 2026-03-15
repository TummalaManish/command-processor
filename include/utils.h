#pragma once

#include <mutex>

using lk_gd = std::lock_guard<std::mutex>;
using unq_lk = std::unique_lock<std::mutex>;
