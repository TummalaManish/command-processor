#pragma once

//
//
//
//

namespace cmd {

class Command final {

public:
  /*
   * @brief the processor runs the step. A step has a set of
   *        non-blocking actions.
   * @notes Yes there are problems on how user will compose
   *        the execution into these types of commands.
   */
  virtual void step() = 0;

  /*
   * @breif Returns if the command can be unscheduled.
   */
  virtual bool isReadyToUnschedule() = 0;

  /*
   * @breif Returns if the comamnd ended.
   */
  virtual bool isReadyToExit() = 0;
};

} // namespace cmd
