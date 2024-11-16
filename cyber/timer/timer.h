/******************************************************************************
 * Copyright 2019 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef CYBER_TIMER_TIMER_H_
#define CYBER_TIMER_TIMER_H_

#include <atomic>
#include <memory>

#include "cyber/timer/timing_wheel.h"

namespace apollo {
namespace cyber {

/**
 * @brief The options of timer
 * 定时器选项
 */
struct TimerOption {
  /**
   * @brief Construct a new Timer Option object
   *
   * @param period The period of the timer, unit is ms
   * @param callback The task that the timer needs to perform
   * @param oneshot Oneshot or period
   */
  TimerOption(uint32_t period, std::function<void()> callback, bool oneshot)
      : period(period), callback(callback), oneshot(oneshot) {}

  /**
   * @brief Default constructor for initializer list
   * 默认构造函数
   */
  TimerOption() : period(), callback(), oneshot() {}

  /**
   * @brief The period of the timer, unit is ms
   * 定时器周期，单位为 ms.
   * max: 512 * 64
   * min: 1
   */
  uint32_t period = 0;

  /**The task that the timer needs to perform.
   * 回调函数(定时任务)
   */
  std::function<void()> callback;

  /**
   * True: perform the callback only after the first timing cycle. 单次触发
   * False: perform the callback every timed period. 周期触发(默认)
   */
  bool oneshot;
};

/**
 * @class Timer
 * @brief Used to perform oneshot or periodic timing tasks
 * 定时器类，用于执行单次或周期定时任务.
 */
class Timer {
 public:
  /* 默认构造函数 */
  Timer();

  /**
   * @brief Construct a new Timer object
   * 构造函数
   * @param opt Timer option 定时器选项
   */
  explicit Timer(TimerOption opt);

  /**
   * @brief Construct a new Timer object
   * 构造函数
   * @param period The period of the timer, unit is ms
   * @param callback The tasks that the timer needs to perform
   * @param oneshot True: perform the callback only after the first timing cycle
   *                False: perform the callback every timed period
   */
  Timer(uint32_t period, std::function<void()> callback, bool oneshot);

  /* 析构函数 */
  ~Timer();

  /**
   * @brief Set the Timer Option object
   * 设置定时器选项
   * @param opt The timer option will be set
   */
  void SetTimerOption(TimerOption opt);

  /**
   * @brief Start the timer
   * 启动定时器
   */
  void Start();

  /**
   * @brief Stop the timer
   * 停止定时器
   */
  void Stop();

 private:
  bool InitTimerTask();
  uint64_t timer_id_;                    // 定时器ID
  TimerOption timer_opt_;                // 定时器
  TimingWheel* timing_wheel_ = nullptr;  // 时间轮
  std::shared_ptr<TimerTask> task_;      // 定时任务
  std::atomic<bool> started_ = {false};  // 启动状态
};

}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_TIMER_TIMER_H_
