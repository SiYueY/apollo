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

#include "cyber/timer/timing_wheel.h"

#include <cmath>

#include "cyber/task/task.h"

namespace apollo {
namespace cyber {

/* 启动时间轮 */
void TimingWheel::Start() {
  std::lock_guard<std::mutex> lock(running_mutex_);
  if (!running_) {
    ADEBUG << "TimeWheel start ok";
    /* 更新running_状态 */
    running_ = true;
    /* 启动tick线程 */
    tick_thread_ = std::thread([this]() { this->TickFunc(); });
    scheduler::Instance()->SetInnerThreadAttr("timer", &tick_thread_);
  }
}

/* 停止时间轮 */
void TimingWheel::Shutdown() {
  std::lock_guard<std::mutex> lock(running_mutex_);
  if (running_) {
    /* 更新running_状态 */
    running_ = false;
    /* 等待tick线程结束 */
    if (tick_thread_.joinable()) {
      tick_thread_.join();
    }
  }
}

/* Tick */
void TimingWheel::Tick() {
  auto& bucket = work_wheel_[current_work_wheel_index_];
  {
    /* 加锁 */
    std::lock_guard<std::mutex> lock(bucket.mutex());

    /* 遍历当前TimerBucket中的定时任务 */
    auto ite = bucket.task_list().begin();
    while (ite != bucket.task_list().end()) {
      /* std::weak_ptr::lock() 创建管理被引用的对象的 shared_ptr */
      auto task = ite->lock();
      if (task) {
        ADEBUG << "index: " << current_work_wheel_index_
               << " timer id: " << task->timer_id_;
        /* 回调函数 */
        auto* callback =
            reinterpret_cast<std::function<void()>*>(&(task->callback));
        /* 异步执行 */
        cyber::Async([this, callback] {
          if (this->running_) {
            (*callback)();
          }
        });
      }
      /* 删除已执行的定时任务，迭代器后移 */
      ite = bucket.task_list().erase(ite);
    }
  }
}

/* 添加定时任务 */
void TimingWheel::AddTask(const std::shared_ptr<TimerTask>& task) {
  AddTask(task, current_work_wheel_index_);
}

/* 添加定时任务 */
void TimingWheel::AddTask(const std::shared_ptr<TimerTask>& task,
                          const uint64_t current_work_wheel_index) {
  if (!running_) {
    Start();
  }

  /* 工作时间轮索引 */
  auto work_wheel_index = current_work_wheel_index +
                          static_cast<uint64_t>(std::ceil(
                              static_cast<double>(task->next_fire_duration_ms) /
                              TIMER_RESOLUTION_MS));
  if (work_wheel_index >= WORK_WHEEL_SIZE) {
    auto real_work_wheel_index = GetWorkWheelIndex(work_wheel_index);
    task->remainder_interval_ms = real_work_wheel_index;
    auto assistant_ticks = work_wheel_index / WORK_WHEEL_SIZE;
    if (assistant_ticks == 1 &&
        real_work_wheel_index < current_work_wheel_index_) {
      work_wheel_[real_work_wheel_index].AddTask(task);
      ADEBUG << "add task to work wheel. index :" << real_work_wheel_index;
    } else {
      auto assistant_wheel_index = 0;
      {
        std::lock_guard<std::mutex> lock(current_assistant_wheel_index_mutex_);
        assistant_wheel_index = GetAssistantWheelIndex(
            current_assistant_wheel_index_ + assistant_ticks);
        assistant_wheel_[assistant_wheel_index].AddTask(task);
      }
      ADEBUG << "add task to assistant wheel. index : "
             << assistant_wheel_index;
    }
  } else {
    work_wheel_[work_wheel_index].AddTask(task);
    ADEBUG << "add task [" << task->timer_id_
           << "] to work wheel. index :" << work_wheel_index;
  }
}

void TimingWheel::Cascade(const uint64_t assistant_wheel_index) {
  auto& bucket = assistant_wheel_[assistant_wheel_index];
  std::lock_guard<std::mutex> lock(bucket.mutex());
  auto ite = bucket.task_list().begin();
  while (ite != bucket.task_list().end()) {
    auto task = ite->lock();
    if (task) {
      work_wheel_[task->remainder_interval_ms].AddTask(task);
    }
    ite = bucket.task_list().erase(ite);
  }
}

/* Tick 函数 */
void TimingWheel::TickFunc() {
  Rate rate(TIMER_RESOLUTION_MS * 1000000);  // ms to ns
  while (running_) {
    Tick();
    // AINFO_EVERY(1000) << "Tick " << TickCount();
    tick_count_++;
    rate.Sleep();
    {
      std::lock_guard<std::mutex> lock(current_work_wheel_index_mutex_);
      current_work_wheel_index_ =
          GetWorkWheelIndex(current_work_wheel_index_ + 1);
    }
    if (current_work_wheel_index_ == 0) {
      {
        std::lock_guard<std::mutex> lock(current_assistant_wheel_index_mutex_);
        current_assistant_wheel_index_ =
            GetAssistantWheelIndex(current_assistant_wheel_index_ + 1);
      }
      Cascade(current_assistant_wheel_index_);
    }
  }
}

/* 构造函数 */
TimingWheel::TimingWheel() {}

}  // namespace cyber
}  // namespace apollo
