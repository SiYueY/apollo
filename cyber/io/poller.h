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

#ifndef CYBER_IO_POLLER_H_
#define CYBER_IO_POLLER_H_

#include <atomic>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "cyber/base/atomic_rw_lock.h"
#include "cyber/common/macros.h"
#include "cyber/io/poll_data.h"

namespace apollo {
namespace cyber {
namespace io {

/* Poller */
class Poller {
 public:
  /* Request Pointer */
  using RequestPtr = std::shared_ptr<PollRequest>;
  /* Request Map */
  using RequestMap = std::unordered_map<int, RequestPtr>;
  /* Control Parameter Map */
  using CtrlParamMap = std::unordered_map<int, PollCtrlParam>;

  /* 析构函数 */
  virtual ~Poller();

  /* 关闭 */
  void Shutdown();

  /* 注册/注销 */
  bool Register(const PollRequest& req);
  bool Unregister(const PollRequest& req);

 private:
  /* 初始化 */
  bool Init();
  /* 清空 */
  void Clear();
  /* Poll 轮训 */
  void Poll(int timeout_ms);
  /* 线程函数 */
  void ThreadFunc();
  /* 处理事件 */
  void HandleChanges();
  /* 获取超时时间 */
  int GetTimeoutMs();
  /* 通知 */
  void Notify();

  int epoll_fd_ = -1;
  /* 线程 */
  std::thread thread_;
  /* 是否关闭 */
  std::atomic<bool> is_shutdown_ = {true};

  /* 管道 */
  int pipe_fd_[2] = {-1, -1};
  /* 管道互斥锁 */
  std::mutex pipe_mutex_;

  /* 请求 Map */
  RequestMap requests_;
  CtrlParamMap ctrl_params_;
  /* 读写锁 */
  base::AtomicRWLock poll_data_lock_;

  /* Poll Size */
  const int kPollSize = 32;
  /* Poll Timeout */
  const int kPollTimeoutMs = 100;

  /* 单例模式 */
  DECLARE_SINGLETON(Poller)
};

}  // namespace io
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_IO_POLLER_H_
