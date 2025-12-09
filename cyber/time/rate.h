/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
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

#ifndef CYBER_TIME_RATE_H_
#define CYBER_TIME_RATE_H_

#include "cyber/time/duration.h"
#include "cyber/time/time.h"

namespace apollo {
namespace cyber {

/* Rate 速率 */
class Rate {
 public:
  /* 构造函数 */
  explicit Rate(double frequency);
  explicit Rate(uint64_t nanoseconds);
  explicit Rate(const Duration&);
  /* 休眠 */
  void Sleep();
  /* 重置 */
  void Reset();
  /* 循环时间 */
  Duration CycleTime() const;
  /* 期望循环时间 */
  Duration ExpectedCycleTime() const { return expected_cycle_time_; }

 private:
  /* 开始时间 */
  Time start_;
  /* 期望循环时间 */
  Duration expected_cycle_time_;
  /* 实际循环时间 */
  Duration actual_cycle_time_;
};

}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_TIME_RATE_H_
