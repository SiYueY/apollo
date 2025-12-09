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

#ifndef CYBER_BASE_UNBOUNDED_QUEUE_H_
#define CYBER_BASE_UNBOUNDED_QUEUE_H_

#include <unistd.h>

#include <atomic>
#include <cstdint>
#include <memory>

namespace apollo {
namespace cyber {
namespace base {

/* 无锁无界队列 */
template <typename T>
class UnboundedQueue {
 public:
  /* 构造函数 */
  UnboundedQueue() { Reset(); }

  /* 禁用拷贝构造函数和拷贝赋值运算符 */
  UnboundedQueue& operator=(const UnboundedQueue& other) = delete;
  UnboundedQueue(const UnboundedQueue& other) = delete;

  /* 析构函数 */
  ~UnboundedQueue() { Destroy(); }

  /* 清空 */
  void Clear() {
    Destroy();
    Reset();
  }

  /* 入队 */
  void Enqueue(const T& element) {
    auto node = new Node();
    node->data = element;
    Node* old_tail = tail_.load();

    while (true) {
      if (tail_.compare_exchange_strong(old_tail, node)) {
        old_tail->next = node;
        old_tail->release();
        size_.fetch_add(1);
        break;
      }
    }
  }

  /* 出队 */
  bool Dequeue(T* element) {
    Node* old_head = head_.load();
    Node* head_next = nullptr;
    do {
      head_next = old_head->next;

      if (head_next == nullptr) {
        return false;
      }
    } while (!head_.compare_exchange_strong(old_head, head_next));
    *element = head_next->data;
    size_.fetch_sub(1);
    old_head->release();
    return true;
  }

  /* 队列大小 */
  size_t Size() { return size_.load(); }

  /* 是否为空 */
  bool Empty() { return size_.load() == 0; }

 private:
  /* Node 节点结构 */
  struct Node {
    T data;
    std::atomic<uint32_t> ref_count;
    Node* next = nullptr;
    Node() { ref_count.store(2); }
    void release() {
      ref_count.fetch_sub(1);
      if (ref_count.load() == 0) {
        delete this;
      }
    }
  };

  /* 重置 */
  void Reset() {
    auto node = new Node();
    head_.store(node);
    tail_.store(node);
    size_.store(0);
  }

  /* 销毁 */
  void Destroy() {
    auto ite = head_.load();
    Node* tmp = nullptr;
    while (ite != nullptr) {
      tmp = ite->next;
      delete ite;
      ite = tmp;
    }
  }

  /* 队头 */
  std::atomic<Node*> head_;
  /* 队尾 */
  std::atomic<Node*> tail_;
  /* 队列大小 */
  std::atomic<size_t> size_;
};

}  // namespace base
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_BASE_UNBOUNDED_QUEUE_H_
