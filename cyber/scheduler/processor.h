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

#ifndef CYBER_SCHEDULER_PROCESSOR_H_
#define CYBER_SCHEDULER_PROCESSOR_H_

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "cyber/proto/scheduler_conf.pb.h"

#include "cyber/croutine/croutine.h"
#include "cyber/scheduler/processor_context.h"

namespace apollo {
namespace cyber {
namespace scheduler {

using croutine::CRoutine;

/* 快照 snapshot */
struct Snapshot {
  /* 执行开始时间 */
  std::atomic<uint64_t> execute_start_time = {0};
  /* 执行器id */
  std::atomic<pid_t> processor_id = {0};
  /* 协程名称 */
  std::string routine_name;
};

/* 执行器 Processor */
class Processor {
 public:
  /* 构造函数 */
  Processor();
  /* 析构函数 */
  virtual ~Processor();

  /* 运行 */
  void Run();
  /* 停止 */
  void Stop();
  /* 绑定上下文 */
  void BindContext(const std::shared_ptr<ProcessorContext>& context);
  /* 获取线程 */
  std::thread* Thread() { return &thread_; }
  /* 获取线程id */
  std::atomic<pid_t>& Tid();

  /* 执行器快照 */
  std::shared_ptr<Snapshot> ProcSnapshot() { return snap_shot_; }

 private:
  /* 执行器上下文 */
  std::shared_ptr<ProcessorContext> context_;
  /* 条件变量 */
  std::condition_variable cv_ctx_;
  std::once_flag thread_flag_;
  /* 互斥锁 */
  std::mutex mtx_ctx_;
  /* 线程 */
  std::thread thread_;
  /* 线程id */
  std::atomic<pid_t> tid_{-1};
  /* 运行状态 */
  std::atomic<bool> running_{false};
  /* 快照 */
  std::shared_ptr<Snapshot> snap_shot_ = std::make_shared<Snapshot>();
};

}  // namespace scheduler
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_SCHEDULER_PROCESSOR_H_
