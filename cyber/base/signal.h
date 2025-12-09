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

#ifndef CYBER_BASE_SIGNAL_H_
#define CYBER_BASE_SIGNAL_H_

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <mutex>

namespace apollo {
namespace cyber {
namespace base {

template <typename... Args>
class Slot;

template <typename... Args>
class Connection;

/* 信号Signal */
template <typename... Args>
class Signal {
 public:
  using Callback = std::function<void(Args...)>;
  using SlotPtr = std::shared_ptr<Slot<Args...>>;
  using SlotList = std::list<SlotPtr>;
  using ConnectionType = Connection<Args...>;

  /* 构造函数 */
  Signal() {}
  /* 析构函数 */
  virtual ~Signal() { DisconnectAllSlots(); }

  /* 重载()运算符 */
  void operator()(Args... args) {
    SlotList local;
    {
      std::lock_guard<std::mutex> lock(mutex_);
      for (auto& slot : slots_) {
        local.emplace_back(slot);
      }
    }

    if (!local.empty()) {
      for (auto& slot : local) {
        /* 执行槽函数 */
        (*slot)(args...);
      }
    }

    ClearDisconnectedSlots();
  }

  /* Connect连接信号和槽函数 */
  ConnectionType Connect(const Callback& cb) {
    auto slot = std::make_shared<Slot<Args...>>(cb);
    {
      std::lock_guard<std::mutex> lock(mutex_);
      slots_.emplace_back(slot);
    }

    return ConnectionType(slot, this);
  }

  /* Disconnect断开信号和槽函数的连接 */
  bool Disconnect(const ConnectionType& conn) {
    bool find = false;
    {
      std::lock_guard<std::mutex> lock(mutex_);
      for (auto& slot : slots_) {
        if (conn.HasSlot(slot)) {
          find = true;
          slot->Disconnect();
        }
      }
    }

    if (find) {
      ClearDisconnectedSlots();
    }
    return find;
  }

  /* DisconnectAllSlots断开所有信号和槽函数的连接 */
  void DisconnectAllSlots() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& slot : slots_) {
      slot->Disconnect();
    }
    slots_.clear();
  }

 private:
  /* 禁止拷贝构造函数和拷贝赋值运算符 */
  Signal(const Signal&) = delete;
  Signal& operator=(const Signal&) = delete;

  /* ClearDisconnectedSlots清除已经断开的槽函数 */
  void ClearDisconnectedSlots() {
    std::lock_guard<std::mutex> lock(mutex_);
    slots_.erase(
        std::remove_if(slots_.begin(), slots_.end(),
                       [](const SlotPtr& slot) { return !slot->connected(); }),
        slots_.end());
  }

  /* 信号槽函数列表 */
  SlotList slots_;
  /* 互斥锁 */
  std::mutex mutex_;
};

template <typename... Args>
class Connection {
 public:
  /* 槽函数指针*/
  using SlotPtr = std::shared_ptr<Slot<Args...>>;
  /* 信号指针 */
  using SignalPtr = Signal<Args...>*;

  /* 构造函数 */
  Connection() : slot_(nullptr), signal_(nullptr) {}
  /* 构造函数 */
  Connection(const SlotPtr& slot, const SignalPtr& signal)
      : slot_(slot), signal_(signal) {}
  /* 析构函数 */
  virtual ~Connection() {
    slot_ = nullptr;
    signal_ = nullptr;
  }

  /* 重载赋值运算符 */
  Connection& operator=(const Connection& another) {
    if (this != &another) {
      this->slot_ = another.slot_;
      this->signal_ = another.signal_;
    }
    return *this;
  }

  /* 槽函数指针是否有效 */
  bool HasSlot(const SlotPtr& slot) const {
    if (slot != nullptr && slot_ != nullptr) {
      return slot_.get() == slot.get();
    }
    return false;
  }

  /* 槽函数是否连接到信号 */
  bool IsConnected() const {
    if (slot_) {
      return slot_->connected();
    }
    return false;
  }

  /* 断开连接 */
  bool Disconnect() {
    if (signal_ && slot_) {
      return signal_->Disconnect(*this);
    }
    return false;
  }

 private:
  SlotPtr slot_;
  SignalPtr signal_;
};

/* 槽函数Slot */
template <typename... Args>
class Slot {
 public:
  /* 回调函数类型 */
  using Callback = std::function<void(Args...)>;
  /* 构造函数 */
  Slot(const Slot& another)
      : cb_(another.cb_), connected_(another.connected_) {}
  /* 构造函数 */
  explicit Slot(const Callback& cb, bool connected = true)
      : cb_(cb), connected_(connected) {}
  /* 析构函数 */
  virtual ~Slot() {}

  /* 重载()运算符 */
  void operator()(Args... args) {
    if (connected_ && cb_) {
      cb_(args...);
    }
  }

  /* 断开连接 */
  void Disconnect() { connected_ = false; }
  /* 是否连接 */
  bool connected() const { return connected_; }

 private:
  /* 回调函数 */
  Callback cb_;
  /* 是否连接 */
  bool connected_ = true;
};

}  // namespace base
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_BASE_SIGNAL_H_
