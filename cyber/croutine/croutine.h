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

#ifndef CYBER_CROUTINE_CROUTINE_H_
#define CYBER_CROUTINE_CROUTINE_H_

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <set>
#include <string>

#include "cyber/common/log.h"
#include "cyber/croutine/detail/routine_context.h"

namespace apollo {
namespace cyber {
namespace croutine {

/* 协程函数类型 */
using RoutineFunc = std::function<void()>;
/* 时间类型 */
using Duration = std::chrono::microseconds;

/* 协程状态 */
enum class RoutineState { 
  READY,     // 就绪
  FINISHED,  // 完成
  SLEEP,     // 睡眠
  IO_WAIT,   // 等待 IO
  DATA_WAIT  // 等待 Data
};

/* 协程 */
class CRoutine {
 public:
  /* 构造函数 */
  explicit CRoutine(const RoutineFunc &func);
  /* 析构函数 */
  virtual ~CRoutine();

  // 暂停
  static void Yield();
  static void Yield(const RoutineState &state);

  /* 设置主协程上下文 */
  static void SetMainContext(const std::shared_ptr<RoutineContext> &context);
  /* 获取当前协程对象 */
  static CRoutine *GetCurrentRoutine();
  /* 获取主协程栈指针 */
  static char **GetMainStack();

  // 加锁/解锁
  bool Acquire();
  void Release();

  // It is caller's responsibility to check if state_ is valid before calling SetUpdateFlag().
  // 设置协程更新标志, 调用之前需要确保状态有效
  void SetUpdateFlag();

  // acquire && release should be called before Resume when work-steal like mechanism used
  /* 恢复 */
  RoutineState Resume();
  /* 更新协程状态 */
  RoutineState UpdateState();
  /* 获取协程上下文 */
  RoutineContext *GetContext();
  /* 获取协程栈 */
  char **GetStack();

  /* 运行 */
  void Run();
  /* 停止 */
  void Stop();
  /* 唤醒 */
  void Wake();
  /* 挂起 */
  void HangUp();
  /* 休眠 */
  void Sleep(const Duration &sleep_duration);

  /* 获取/设置协程状态 */
  RoutineState state() const;
  void set_state(const RoutineState &state);

  /* 获取/设置协程ID */
  uint64_t id() const;
  void set_id(uint64_t id);

  /* 获取/设置协程名称 */
  const std::string &name() const;
  void set_name(const std::string &name);

  /* 获取/设置 Procrssor ID */
  int processor_id() const;
  void set_processor_id(int processor_id);

  /* 获取/设置协程优先级 */
  uint32_t priority() const;
  void set_priority(uint32_t priority);

  /* 获取唤醒时间 */
  std::chrono::steady_clock::time_point wake_time() const;

  /* 设置/获取组别名称 */
  void set_group_name(const std::string &group_name) {
    group_name_ = group_name;
  }
  const std::string &group_name() { return group_name_; }

 private:
  /* 禁用拷贝构造函数和拷贝赋值运算符 */
  CRoutine(CRoutine &) = delete;
  CRoutine &operator=(CRoutine &) = delete;

  /* 协程名称 */
  std::string name_;
  /* 唤醒时间 */
  std::chrono::steady_clock::time_point wake_time_ =
      std::chrono::steady_clock::now();

  /* 协程执行函数 */
  RoutineFunc func_;
  /* 协程状态 */
  RoutineState state_;

  /* 协程上下文 */
  std::shared_ptr<RoutineContext> context_;

  /* 锁 */
  std::atomic_flag lock_ = ATOMIC_FLAG_INIT;
  /* 更新标志 */
  std::atomic_flag updated_ = ATOMIC_FLAG_INIT;

  /* 强制停止标志 */
  bool force_stop_ = false;

  /* Processor ID */
  int processor_id_ = -1;
  /* 优先级 */
  uint32_t priority_ = 0;
  /* 协程 ID */
  uint64_t id_ = 0;

  /* 组别名称 */
  std::string group_name_;

  /* 当前协程对象 */
  static thread_local CRoutine *current_routine_;
  /* 主协程栈指针 */
  static thread_local char *main_stack_;
};

inline void CRoutine::Yield(const RoutineState &state) {
  auto routine = GetCurrentRoutine();
  routine->set_state(state);
  SwapContext(GetCurrentRoutine()->GetStack(), GetMainStack());
}

inline void CRoutine::Yield() {
  SwapContext(GetCurrentRoutine()->GetStack(), GetMainStack());
}

inline CRoutine *CRoutine::GetCurrentRoutine() { return current_routine_; }

inline char **CRoutine::GetMainStack() { return &main_stack_; }

inline RoutineContext *CRoutine::GetContext() { return context_.get(); }

inline char **CRoutine::GetStack() { return &(context_->sp); }

inline void CRoutine::Run() { func_(); }

inline void CRoutine::set_state(const RoutineState &state) { state_ = state; }

inline RoutineState CRoutine::state() const { return state_; }

inline std::chrono::steady_clock::time_point CRoutine::wake_time() const {
  return wake_time_;
}

inline void CRoutine::Wake() { state_ = RoutineState::READY; }

inline void CRoutine::HangUp() { CRoutine::Yield(RoutineState::DATA_WAIT); }

inline void CRoutine::Sleep(const Duration &sleep_duration) {
  wake_time_ = std::chrono::steady_clock::now() + sleep_duration;
  CRoutine::Yield(RoutineState::SLEEP);
}

inline uint64_t CRoutine::id() const { return id_; }

inline void CRoutine::set_id(uint64_t id) { id_ = id; }

inline const std::string &CRoutine::name() const { return name_; }

inline void CRoutine::set_name(const std::string &name) { name_ = name; }

inline int CRoutine::processor_id() const { return processor_id_; }

inline void CRoutine::set_processor_id(int processor_id) {
  processor_id_ = processor_id;
}

inline RoutineState CRoutine::UpdateState() {
  // Synchronous Event Mechanism
  if (state_ == RoutineState::SLEEP &&
      std::chrono::steady_clock::now() > wake_time_) {
    state_ = RoutineState::READY;
    return state_;
  }

  // Asynchronous Event Mechanism
  if (!updated_.test_and_set(std::memory_order_release)) {
    if (state_ == RoutineState::DATA_WAIT || state_ == RoutineState::IO_WAIT) {
      state_ = RoutineState::READY;
    }
  }
  return state_;
}

inline uint32_t CRoutine::priority() const { return priority_; }

inline void CRoutine::set_priority(uint32_t priority) { priority_ = priority; }

inline bool CRoutine::Acquire() {
  return !lock_.test_and_set(std::memory_order_acquire);
}

inline void CRoutine::Release() {
  return lock_.clear(std::memory_order_release);
}

inline void CRoutine::SetUpdateFlag() {
  updated_.clear(std::memory_order_release);
}

}  // namespace croutine
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_CROUTINE_CROUTINE_H_
