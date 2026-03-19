#pragma once

#include <memory>

#ifndef JOB_SOB_OPT_SIZ
#define JOB_SOB_OPT_SIZ 128
#endif

namespace utl {
template <typename Signature> class Job;

template <typename Ret_t, typename... Args_t> class Job<Ret_t(Args_t...)> {
  class Concept {
  public:
    virtual ~Concept() = default;

    virtual void copyTo(/* TODO: Args? */) = 0;

    virtual void moveTo(/* TODO: Args?*/) = 0;
  };

  template <typename Ptr_t> class Deleter {
    void operator()(Ptr_t);

  public:
  };

  using Uptr = std::unique_ptr<void>;

public:
  template <typename Closue_t> class Model final : public Concept {
  public:
    virtual ~Concept() = default;
    virtual void copyTo(/* TODO: Args? */) {}

    virtual void moveTo(/* TODO: Args?*/) {}
  };
};
} // namespace utl
