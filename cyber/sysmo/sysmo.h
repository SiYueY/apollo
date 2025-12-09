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

#ifndef CYBER_SYSMO_SYSMO_H_
#define CYBER_SYSMO_SYSMO_H_

#include <chrono>
#include <condition_variable>
#include <list>
#include <mutex>
#include <string>
#include <thread>

#include "cyber/scheduler/scheduler_factory.h"

namespace apollo {
namespace cyber {

using apollo::cyber::scheduler::Scheduler;

/* Systems Monitor */
class SysMo {
 public:
  /* 启动 */
  void Start();
  /* 关闭 */
  void Shutdown();

 private:
  /* 检查 */
  void Checker();

  /* 是否关闭 */
  std::atomic<bool> shut_down_{false};
  /* 是否启动 */
  bool start_ = false;

  /* 时间间隔(ms) */
  int sysmo_interval_ms_ = 100;
  /* 条件变量 */
  std::condition_variable cv_;
  /* 互斥锁 */
  std::mutex lk_;
  /* 线程 */
  std::thread sysmo_;

  /* 单例模式 */
  DECLARE_SINGLETON(SysMo);
};

}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_SYSMO_SYSMO_H_
