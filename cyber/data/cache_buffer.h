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

#ifndef CYBER_DATA_CACHE_BUFFER_H_
#define CYBER_DATA_CACHE_BUFFER_H_

#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace apollo {
namespace cyber {
namespace data {

/* 缓存缓冲区 CacheBuffer：线程安全的环形缓冲区模板类，用于存储数据对象 T. */
template <typename T>
class CacheBuffer {
 public:
  /* 数据对象的类型，即模板参数 T */
  using value_type = T;
  /* 用于索引和大小的无符号整型 */
  using size_type = std::size_t;
  /* 函数类型的别名，表示融合回调函数 */
  using FusionCallback = std::function<void(const T&)>;

  /* 拷贝构造函数 */
  explicit CacheBuffer(uint64_t size) {
    capacity_ = size + 1;
    buffer_.resize(capacity_);
  }

  /* 构造函数 */
  CacheBuffer(const CacheBuffer& rhs) {
    std::lock_guard<std::mutex> lg(rhs.mutex_);
    head_ = rhs.head_;
    tail_ = rhs.tail_;
    buffer_ = rhs.buffer_;
    capacity_ = rhs.capacity_;
    fusion_callback_ = rhs.fusion_callback_;
  }

  T& operator[](const uint64_t& pos) { return buffer_[GetIndex(pos)]; }
  const T& at(const uint64_t& pos) const { return buffer_[GetIndex(pos)]; }

  /* 缓冲区头部 head 的索引 */
  uint64_t Head() const { return head_ + 1; }
  /* 缓冲区尾部 tail 的索引 */
  uint64_t Tail() const { return tail_; }
  /* 缓冲区中的元素数量 */
  uint64_t Size() const { return tail_ - head_; }

  /* 缓冲区前端元素 */
  const T& Front() const { return buffer_[GetIndex(head_ + 1)]; }
  /* 缓冲区尾端元素 */
  const T& Back() const { return buffer_[GetIndex(tail_)]; }

  /* 检查缓冲区是否为空 */
  bool Empty() const { return tail_ == 0; }
  /* 检查缓冲区是否已满 */
  bool Full() const { return capacity_ - 1 == tail_ - head_; }
  /* 返回缓冲区的总容量 */
  uint64_t Capacity() const { return capacity_; }

  /* 设置融合回调函数，当新元素填入时调用 */
  void SetFusionCallback(const FusionCallback& callback) {
    fusion_callback_ = callback;
  }

  /* 向缓冲区添加新元素.
   * 若设置融合回调，则调用回调函数；否则，将元素添加到缓冲区并更新头尾索引. */
  void Fill(const T& value) {
    if (fusion_callback_) {
      fusion_callback_(value);
    } else {
      if (Full()) {
        buffer_[GetIndex(head_)] = value;
        ++head_;
        ++tail_;
      } else {
        buffer_[GetIndex(tail_ + 1)] = value;
        ++tail_;
      }
    }
  }

  /* 互斥锁 */
  std::mutex& Mutex() { return mutex_; }

 private:
  CacheBuffer& operator=(const CacheBuffer& other) = delete;
  uint64_t GetIndex(const uint64_t& pos) const { return pos % capacity_; }

  /* 缓冲区头部索引 head */
  uint64_t head_ = 0;
  /* 缓冲区尾部索引 tail */
  uint64_t tail_ = 0;
  /* 缓冲区容量 */
  uint64_t capacity_ = 0;
  /* 存储数据元素 */
  std::vector<T> buffer_;
  /* 互斥锁 */
  mutable std::mutex mutex_;
  /* 融合回调函数 */
  FusionCallback fusion_callback_;
};

}  // namespace data
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_DATA_CACHE_BUFFER_H_
