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

#ifndef CYBER_NODE_READER_BASE_H_
#define CYBER_NODE_READER_BASE_H_

#include <atomic>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "cyber/common/macros.h"
#include "cyber/common/util.h"
#include "cyber/event/perf_event_cache.h"
#include "cyber/transport/transport.h"

namespace apollo {
namespace cyber {

using apollo::cyber::common::GlobalData;
using apollo::cyber::event::PerfEventCache;
using apollo::cyber::event::TransPerf;

/**
 * @class ReaderBase Reader基类
 * @brief Base Class for Reader
 * Reader is identified by one apollo::cyber::proto::RoleAttribute,
 * it contains the channel_name, channel_id that we subscribe,
 * and host_name, process_id and node that we are located,
 * and qos that describes our transportation quality.
 * qos: Quality of Service, 描述消息传输的服务质量.
 */
class ReaderBase {
 public:
  /* 构造函数 */
  explicit ReaderBase(const proto::RoleAttributes& role_attr)
      : role_attr_(role_attr), init_(false) {}
  /* 析构函数 */
  virtual ~ReaderBase() {}

  /**
   * @brief Init the Reader object
   * 初始化Reader对象
   * @return true if init successfully
   * @return false if init failed
   */
  virtual bool Init() = 0;

  /**
   * @brief Shutdown the Reader object
   * 关闭Reader对象
   */
  virtual void Shutdown() = 0;

  /**
   * @brief Clear local data
   * 清除本地数据
   */
  virtual void ClearData() = 0;

  /**
   * @brief Get stored data
   * 获取存储数据
   */
  virtual void Observe() = 0;

  /**
   * @brief Query whether the Reader has data to be handled
   * 查询Reader是否存在待处理数据
   * @return true if data container is empty
   * @return false if data container has data
   */
  virtual bool Empty() const = 0;

  /**
   * @brief Query whether we have received data since last clear
   * 查询自上次清除以来是否接收到数据
   * @return true if the reader has received data
   * @return false if the reader has not received data
   */
  virtual bool HasReceived() const = 0;

  /**
   * @brief Get time interval of since last receive message
   * 获取自上次接收到消息以来的时间间隔
   * @return double seconds delay
   */
  virtual double GetDelaySec() const = 0;

  /**
   * @brief Get the value of pending queue size
   * 获取待处理队列大小
   * @return uint32_t result value
   */
  virtual uint32_t PendingQueueSize() const = 0;

  /**
   * @brief Query is there any writer that publish the subscribed channel
   * 查询是否存在发布订阅Channel的Writer
   * @return true if there is at least one Writer publish the channel
   * @return false if there is no Writer publish the channel
   */
  virtual bool HasWriter() { return false; }

  /**
   * @brief Get all writers pushlish the channel we subscribes
   * 获取所有发布订阅Channel的Writer的RoleAttributes
   * @param writers result RoleAttributes vector
   */
  virtual void GetWriters(std::vector<proto::RoleAttributes>* writers) {}

  /**
   * @brief Get Reader's Channel name
   * 获取Reader的Channel名称
   * @return const std::string& channel name
   */
  const std::string& GetChannelName() const {
    return role_attr_.channel_name();
  }

  /**
   * @brief Get Reader's Channel id
   * 获取Reader的Channel ID
   * @return uint64_t channel id
   */
  uint64_t ChannelId() const { return role_attr_.channel_id(); }

  /**
   * @brief Get qos profile. You can see qos description
   * 获取Qos配置文件
   * @return const proto::QosProfile& result qos
   */
  const proto::QosProfile& QosProfile() const {
    return role_attr_.qos_profile();
  }

  /**
   * @brief Query whether the Reader is initialized
   * 查询Reader是否已初始化
   * @return true if the Reader has been inited
   * @return false if the Reader has not been inited
   */
  bool IsInit() const { return init_.load(); }

 protected:
  /* RoleAttribute */
  proto::RoleAttributes role_attr_;
  /* init flag */
  std::atomic<bool> init_;
};

/**
 * @brief One Channel is related to one Receiver. 一个Channel对应一个Receiver
 * ReceiverManager is in charge of attaching one Receiver to its responding
 * Channel. We pass a DataDispatcher's callback func to this Receiver so when a
 * message is received, it will be push to the `ChannelBuffer`, and
 * `DataVisitor` will `Fetch` data and pass to `Reader`'s callback func
 * ReceiverManager管理一个Receiver与其相应Channel之间的关联关系.
 * 将DataDispatcher的回调函数传递给Receiver, 当接收到消息时, 将其推送到ChannelBuffer,
 * DataVisitor将Fetch数据并传递给Reader的回调函数.
 * @tparam MessageT Message Type.
 */
template <typename MessageT>
class ReceiverManager {
 public:
  /* 析构函数 */
  ~ReceiverManager() { receiver_map_.clear(); }

  /**
   * @brief Get the Receiver object
   * 获取Receiver对象
   * @param role_attr the attribute that the Receiver has RoleAttributes
   * @return std::shared_ptr<transport::Receiver<MessageT>> result Receiver
   */
  auto GetReceiver(const proto::RoleAttributes& role_attr) ->
      typename std::shared_ptr<transport::Receiver<MessageT>>;

 private:
  /* Receiver Map */
  std::unordered_map<std::string,
                     typename std::shared_ptr<transport::Receiver<MessageT>>>
      receiver_map_;
  /* Receiver Map 互斥锁 */
  std::mutex receiver_map_mutex_;

  /* 单例模式 */
  DECLARE_SINGLETON(ReceiverManager<MessageT>)
};

/**
 * @brief Construct a new Receiver Manager< Message T>:: Receiver Manager object
 * 构造函数
 * @tparam MessageT param
 */
template <typename MessageT>
ReceiverManager<MessageT>::ReceiverManager() {}

template <typename MessageT>
auto ReceiverManager<MessageT>::GetReceiver(
    const proto::RoleAttributes& role_attr) ->
    typename std::shared_ptr<transport::Receiver<MessageT>> {
  std::lock_guard<std::mutex> lock(receiver_map_mutex_);
  // because multi reader for one channel will write datacache multi times,
  // so reader for datacache we use map to keep one instance for per channel
  const std::string& channel_name = role_attr.channel_name();
  if (receiver_map_.count(channel_name) == 0) {
    receiver_map_[channel_name] =
        transport::Transport::Instance()->CreateReceiver<MessageT>(
            role_attr, [](const std::shared_ptr<MessageT>& msg,
                          const transport::MessageInfo& msg_info,
                          const proto::RoleAttributes& reader_attr) {
              (void)msg_info;
              (void)reader_attr;
              PerfEventCache::Instance()->AddTransportEvent(
                  TransPerf::DISPATCH, reader_attr.channel_id(),
                  msg_info.seq_num());
              data::DataDispatcher<MessageT>::Instance()->Dispatch(
                  reader_attr.channel_id(), msg);
              PerfEventCache::Instance()->AddTransportEvent(
                  TransPerf::NOTIFY, reader_attr.channel_id(),
                  msg_info.seq_num());
            });
  }
  return receiver_map_[channel_name];
}

}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_NODE_READER_BASE_H_
