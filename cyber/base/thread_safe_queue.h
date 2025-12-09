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

#ifndef CYBER_BASE_THREAD_SAFE_QUEUE_H_
#define CYBER_BASE_THREAD_SAFE_QUEUE_H_

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

namespace apollo {
namespace cyber {
namespace base {

/* 线程安全队列 */
template <typename T>
class ThreadSafeQueue {
 public:
  /* 构造函数 */
  ThreadSafeQueue() {}

  /* 禁止拷贝构造函数和拷贝赋值运算符 */
  ThreadSafeQueue& operator=(const ThreadSafeQueue& other) = delete;
  ThreadSafeQueue(const ThreadSafeQueue& other) = delete;

  /* 析构函数 */
  ~ThreadSafeQueue() { BreakAllWait(); }

  /* 入队 */
  void Enqueue(const T& element) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.emplace(element);
    cv_.notify_one();
  }

  /* 出队 */
  bool Dequeue(T* element) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) {
      return false;
    }
    *element = std::move(queue_.front());
    queue_.pop();
    return true;
  }

  /* 等待出队 */
  bool WaitDequeue(T* element) {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this]() { return break_all_wait_ || !queue_.empty(); });
    if (break_all_wait_) {
      return false;
    }
    *element = std::move(queue_.front());
    queue_.pop();
    return true;
  }

  typename std::queue<T>::size_type Size() {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
  }

  /* 队列是否为空 */
  bool Empty() {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
  }

  /* 打破所有等待 */
  void BreakAllWait() {
    break_all_wait_ = true;
    cv_.notify_all();
  }

 private:
  /* 是否打破所有等待 */
  volatile bool break_all_wait_ = false;
  /* 互斥锁 */
  std::mutex mutex_;
  /* 队列 */
  std::queue<T> queue_;
  /* 条件变量 */
  std::condition_variable cv_;
};

}  // namespace base
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_BASE_THREAD_SAFE_QUEUE_H_
