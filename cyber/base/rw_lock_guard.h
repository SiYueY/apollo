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

#ifndef CYBER_BASE_RW_LOCK_GUARD_H_
#define CYBER_BASE_RW_LOCK_GUARD_H_

#include <unistd.h>

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>

namespace apollo {
namespace cyber {
namespace base {

/* Read Lock Guard */
template <typename RWLock>
class ReadLockGuard {
 public:
  /* 构造函数 */
  explicit ReadLockGuard(RWLock& lock) : rw_lock_(lock) { rw_lock_.ReadLock(); }

  /* 析构函数 */
  ~ReadLockGuard() { rw_lock_.ReadUnlock(); }

 private:
  /* 禁止拷贝构造函数和拷贝赋值运算符 */
  ReadLockGuard(const ReadLockGuard& other) = delete;
  ReadLockGuard& operator=(const ReadLockGuard& other) = delete;

  /* 读锁 */
  RWLock& rw_lock_;
};

/* Write Lock Guard */
template <typename RWLock>
class WriteLockGuard {
 public:
  /* 构造函数 */
  explicit WriteLockGuard(RWLock& lock) : rw_lock_(lock) {
    rw_lock_.WriteLock();
  }

  /* 析构函数 */
  ~WriteLockGuard() { rw_lock_.WriteUnlock(); }

 private:
  /* 禁止拷贝构造函数和拷贝赋值运算符 */
  WriteLockGuard(const WriteLockGuard& other) = delete;
  WriteLockGuard& operator=(const WriteLockGuard& other) = delete;

  /* 写锁 */
  RWLock& rw_lock_;
};

}  // namespace base
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_BASE_RW_LOCK_GUARD_H_
