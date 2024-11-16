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

#ifndef CYBER_TIMER_TIMER_TASK_H_
#define CYBER_TIMER_TIMER_TASK_H_

#include <functional>
#include <mutex>

namespace apollo {
namespace cyber {

class TimerBucket;

/* 定时任务 */
struct TimerTask {
  /* 构造函数 */
  explicit TimerTask(uint64_t timer_id) : timer_id_(timer_id) {}

  uint64_t timer_id_ = 0;              // 定时器ID
  std::function<void()> callback;      // 回调函数
  uint64_t interval_ms = 0;            // 间隔时间
  uint64_t remainder_interval_ms = 0;  // 剩余时间
  uint64_t next_fire_duration_ms = 0;  // 下一次触发时间
  int64_t accumulated_error_ns = 0;    // 累计误差
  uint64_t last_execute_time_ns = 0;   // 上一次执行时间
  std::mutex mutex;                    // 互斥锁
};

}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_TIMER_TIMER_TASK_H_
