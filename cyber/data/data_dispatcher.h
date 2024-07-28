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

#ifndef CYBER_DATA_DATA_DISPATCHER_H_
#define CYBER_DATA_DATA_DISPATCHER_H_

#include <memory>
#include <mutex>
#include <vector>

#include "cyber/common/log.h"
#include "cyber/common/macros.h"
#include "cyber/data/channel_buffer.h"
#include "cyber/state.h"
#include "cyber/time/time.h"

namespace apollo {
namespace cyber {
namespace data {

using apollo::cyber::Time;
using apollo::cyber::base::AtomicHashMap;

/* DataDispatcher 消息分发器: 单例模式，所有数据分发都在 DataDispatcher 中进行.
 * DataDispatcher 会把数据放入对应缓存中，并 Notify 通知对应的协程处理消息 */
template <typename T>
class DataDispatcher {
 public:
  using BufferVector =
      std::vector<std::weak_ptr<CacheBuffer<std::shared_ptr<T>>>>;
  ~DataDispatcher() {}
  // 添加 ChannelBuffer 到 buffers_map_ 中
  void AddBuffer(const ChannelBuffer<T>& channel_buffer);
  // 分发 Channel 中的消息
  bool Dispatch(const uint64_t channel_id, const std::shared_ptr<T>& msg);

 private:
  // DataNotifier 单例模式
  DataNotifier* notifier_ = DataNotifier::Instance();
  // buffers_map_ 互斥锁
  std::mutex buffers_map_mutex_;
  // 哈希表 <key, value>: <Channel id(Topic), 订阅通道消息的 CacheBuffer 数组>
  AtomicHashMap<uint64_t, BufferVector> buffers_map_;
  // 单例模式
  DECLARE_SINGLETON(DataDispatcher)
};

/* 构造函数 */
template <typename T>
inline DataDispatcher<T>::DataDispatcher() {}

/* 向 DataDispatcher 中添加 ChannelBuffer，用于存储和分发消息。 */
template <typename T>
void DataDispatcher<T>::AddBuffer(const ChannelBuffer<T>& channel_buffer) {
  std::lock_guard<std::mutex> lock(buffers_map_mutex_);
  auto buffer = channel_buffer.Buffer();
  BufferVector* buffers = nullptr;
  if (buffers_map_.Get(channel_buffer.channel_id(), &buffers)) {
    buffers->emplace_back(buffer);
  } else {
    BufferVector new_buffers = {buffer};
    buffers_map_.Set(channel_buffer.channel_id(), new_buffers);
  }
}

/* 分发消息到所有注册的 CacheBuffer 中 */
template <typename T>
bool DataDispatcher<T>::Dispatch(const uint64_t channel_id,
                                 const std::shared_ptr<T>& msg) {
  BufferVector* buffers = nullptr;
  if (apollo::cyber::IsShutdown()) {
    return false;
  }
  if (buffers_map_.Get(channel_id, &buffers)) {
    for (auto& buffer_wptr : *buffers) {
      if (auto buffer = buffer_wptr.lock()) {
        std::lock_guard<std::mutex> lock(buffer->Mutex());
        buffer->Fill(msg);
      }
    }
  } else {
    return false;
  }
  return notifier_->Notify(channel_id);
}

}  // namespace data
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_DATA_DATA_DISPATCHER_H_
