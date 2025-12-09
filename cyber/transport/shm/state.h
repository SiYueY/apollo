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

#ifndef CYBER_TRANSPORT_SHM_STATE_H_
#define CYBER_TRANSPORT_SHM_STATE_H_

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>

namespace apollo {
namespace cyber {
namespace transport {

/* 状态 */
class State {
 public:
  /* 构造函数 */
  explicit State(const uint64_t& ceiling_msg_size);
  /* 析构函数 */
  virtual ~State();

  /* 减少引用计数 */
  void DecreaseReferenceCounts() {
    uint32_t current_reference_count = reference_count_.load();
    do {
      if (current_reference_count == 0) {
        return;
      }
    } while (!reference_count_.compare_exchange_strong(
        current_reference_count, current_reference_count - 1));
  }

  /* 增加引用计数 */
  void IncreaseReferenceCounts() { reference_count_.fetch_add(1); }

  /* 获取序列号 */
  uint32_t FetchAddSeq(uint32_t diff) { return seq_.fetch_add(diff); }
  
  /* 获取序列号 */
  uint32_t seq() { return seq_.load(); }

  uint32_t FetchAddArenaSeq(uint32_t diff) {
    return arena_seq_.fetch_add(diff);
  }
  uint32_t arena_seq() { return arena_seq_.load(); }

  void set_need_remap(bool need) { need_remap_.store(need); }
  /* 获取是否需要重新映射 */
  bool need_remap() { return need_remap_; }

  /* 设置消息大小上限 */
  uint64_t ceiling_msg_size() { return ceiling_msg_size_.load(); }
  /* 获取引用计数 */
  uint32_t reference_counts() { return reference_count_.load(); }

 private:
  /* 是否需要重新映射 */
  std::atomic<bool> need_remap_ = {false};
  /* 序列号 */
  std::atomic<uint32_t> seq_ = {0};
  std::atomic<uint32_t> arena_seq_ = {0};
  std::atomic<uint32_t> reference_count_ = {0};
  /* 消息大小上限 */
  std::atomic<uint64_t> ceiling_msg_size_;
};

}  // namespace transport
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_TRANSPORT_SHM_STATE_H_
