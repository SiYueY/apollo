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

#ifndef CYBER_NODE_NODE_H_
#define CYBER_NODE_NODE_H_

#include <map>
#include <memory>
#include <string>
#include <utility>

#include "cyber/node/node_channel_impl.h"
#include "cyber/node/node_service_impl.h"

namespace apollo {
namespace cyber {

template <typename M0, typename M1, typename M2, typename M3>
class Component;
class TimerComponent;

/**
 * @class Node
 * @brief Node is the fundamental building block of Cyber RT.
 * every module contains and communicates through the node.
 * 每个模块都包含一个 node ，并通过该 node 进行通信
 * A module can have different types of communication by defining
 * read/write and/or service/client in a node.
 * @warning Duplicate name is not allowed in topo objects, such as node,
 * reader/writer, service/client in the topo.
 */
class Node {
 public:
  template <typename M0, typename M1, typename M2, typename M3>
  friend class Component;
  friend class TimerComponent;
  friend bool Init(const char*, const std::string&);
  friend std::unique_ptr<Node> CreateNode(const std::string&,
                                          const std::string&);
  virtual ~Node();

  /**
   * @brief Get node's name.  获取 node 的名称
   * @warning duplicate node name is not allowed in the topo.
   * 拓扑结构中的 node 不允许重名
   */
  const std::string& Name() const;

  /**
   * @brief Create a Writer with specific message type. 以特定的 message
   * 类型创建 Writer.
   *
   * @tparam MessageT Message Type.  message 类型
   * @param role_attr is a protobuf message RoleAttributes, which includes the
   * channel name and other info.
   * @return std::shared_ptr<Writer<MessageT>> result Writer Object
   */
  template <typename MessageT>
  auto CreateWriter(const proto::RoleAttributes& role_attr)
      -> std::shared_ptr<Writer<MessageT>>;

  /**
   * @brief Create a Writer with specific message type. 以特定的 message
   * 类型创建 Writer.
   *
   * @tparam MessageT Message Type. message 类型
   * @param channel_name the channel name to be published.  发布的 channel 名称.
   * @return std::shared_ptr<Writer<MessageT>> result Writer Object
   */
  template <typename MessageT>
  auto CreateWriter(const std::string& channel_name)
      -> std::shared_ptr<Writer<MessageT>>;

  /**
   * @brief Create a Reader with specific message type with channel name
   * qos and other configs used will be default.
   *
   * @tparam MessageT Message Type. message 类型
   * @param channel_name the channel of the reader subscribed.  reader 订阅的
   * channel 名称.
   * @param reader_func invoked when message receive. 接收 message
   * 时执行的回调函数. invoked when the message is received.
   * @return std::shared_ptr<cyber::Reader<MessageT>> result Reader Object
   */
  template <typename MessageT>
  auto CreateReader(const std::string& channel_name,
                    const CallbackFunc<MessageT>& reader_func = nullptr)
      -> std::shared_ptr<cyber::Reader<MessageT>>;

  /**
   * @brief Create a Reader with specific message type with reader config
   *
   * @tparam MessageT Message Type. message 类型
   * @param config instance of `ReaderConfig`,
   * include channel name, qos and pending queue size
   * @param reader_func invoked when message receive
   * @return std::shared_ptr<cyber::Reader<MessageT>> result Reader Object
   */
  template <typename MessageT>
  auto CreateReader(const ReaderConfig& config,
                    const CallbackFunc<MessageT>& reader_func = nullptr)
      -> std::shared_ptr<cyber::Reader<MessageT>>;

  /**
   * @brief Create a Reader object with `RoleAttributes`
   *
   * @tparam MessageT Message Type
   * @param role_attr instance of `RoleAttributes`,
   * includes channel name, qos, etc.
   * @param reader_func invoked when message receive
   * @return std::shared_ptr<cyber::Reader<MessageT>> result Reader Object
   */
  template <typename MessageT>
  auto CreateReader(const proto::RoleAttributes& role_attr,
                    const CallbackFunc<MessageT>& reader_func = nullptr)
      -> std::shared_ptr<cyber::Reader<MessageT>>;

  /**
   * @brief Create a Service object with specific `service_name`.
   * 以特定的服务名称创建 Service 对象.
   *
   * @tparam Request Message Type of the Request. Request 消息类型.
   * @tparam Response Message Type of the Response. Response 消息类型.
   * @param service_name specific service name to a serve. 服务的特定名称.
   * @param service_callback invoked when a service is called.
   * 服务被调用时执行的回调函数.
   * @return std::shared_ptr<Service<Request, Response>> result `Service`
   */
  template <typename Request, typename Response>
  auto CreateService(const std::string& service_name,
                     const typename Service<Request, Response>::ServiceCallback&
                         service_callback)
      -> std::shared_ptr<Service<Request, Response>>;

  /**
   * @brief Create a Client object to request Service with `service_name`
   *  创建 Client 对象并请求 `service_name` 服务.
   * @tparam Request Message Type of the Request. Request 消息类型.
   * @tparam Response Message Type of the Response. Response 消息类型.
   * @param service_name specific service name to a Service. 服务的特定名称.
   * @return std::shared_ptr<Client<Request, Response>> result `Client`
   */
  template <typename Request, typename Response>
  auto CreateClient(const std::string& service_name)
      -> std::shared_ptr<Client<Request, Response>>;

  bool DeleteReader(const std::string& channel_name);
  bool DeleteReader(const ReaderConfig& config);
  bool DeleteReader(const proto::RoleAttributes& role_attr);
  /**
   * @brief Observe all readers' data. 观察所有 Reader 的数据.
   */
  void Observe();

  /**
   * @brief clear all readers' data. 清除所有 Reader 的数据.
   */
  void ClearData();

  /**
   * @brief Get the Reader object that subscribe `channel_name`
   *  获取订阅 `channel_name` 的 Reader 对象.
   * @tparam MessageT Message Type. 消息类型.
   * @param channel_name channel name. Channel 名称.
   * @return std::shared_ptr<Reader<MessageT>> result reader
   */
  template <typename MessageT>
  auto GetReader(const std::string& channel_name)
      -> std::shared_ptr<Reader<MessageT>>;

 private:
  explicit Node(const std::string& node_name,
                const std::string& name_space = "");

  std::string node_name_;   // node 名称
  std::string name_space_;  // 命名空间

  std::mutex readers_mutex_;  // reader 互斥锁
  std::map<std::string, std::shared_ptr<ReaderBase>> readers_;

  std::unique_ptr<NodeChannelImpl> node_channel_impl_ =
      nullptr;  // NodeChannelImpl 对象
  std::unique_ptr<NodeServiceImpl> node_service_impl_ =
      nullptr;  // NodeServiceImpl 对象
};

template <typename MessageT>
auto Node::CreateWriter(const proto::RoleAttributes& role_attr)
    -> std::shared_ptr<Writer<MessageT>> {
  return node_channel_impl_->template CreateWriter<MessageT>(role_attr);
}

template <typename MessageT>
auto Node::CreateWriter(const std::string& channel_name)
    -> std::shared_ptr<Writer<MessageT>> {
  return node_channel_impl_->template CreateWriter<MessageT>(channel_name);
}

template <typename MessageT>
auto Node::CreateReader(const proto::RoleAttributes& role_attr,
                        const CallbackFunc<MessageT>& reader_func)
    -> std::shared_ptr<Reader<MessageT>> {
  std::lock_guard<std::mutex> lg(readers_mutex_);
  /* 判断重名 */
  if (readers_.find(role_attr.channel_name()) != readers_.end()) {
    AWARN << "Failed to create reader: reader with the same channel already "
             "exists.";
    return nullptr;
  }
  auto reader = node_channel_impl_->template CreateReader<MessageT>(
      role_attr, reader_func);
  if (reader != nullptr) {
    readers_.emplace(std::make_pair(role_attr.channel_name(), reader));
  }
  return reader;
}

template <typename MessageT>
auto Node::CreateReader(const ReaderConfig& config,
                        const CallbackFunc<MessageT>& reader_func)
    -> std::shared_ptr<cyber::Reader<MessageT>> {
  std::lock_guard<std::mutex> lg(readers_mutex_);
  if (readers_.find(config.channel_name) != readers_.end()) {
    AWARN << "Failed to create reader: reader with the same channel already "
             "exists.";
    return nullptr;
  }
  auto reader =
      node_channel_impl_->template CreateReader<MessageT>(config, reader_func);
  if (reader != nullptr) {
    readers_.emplace(std::make_pair(config.channel_name, reader));
  }
  return reader;
}

template <typename MessageT>
auto Node::CreateReader(const std::string& channel_name,
                        const CallbackFunc<MessageT>& reader_func)
    -> std::shared_ptr<Reader<MessageT>> {
  std::lock_guard<std::mutex> lg(readers_mutex_);
  if (readers_.find(channel_name) != readers_.end()) {
    AWARN << "Failed to create reader: reader with the same channel already "
             "exists.";
    return nullptr;
  }
  auto reader = node_channel_impl_->template CreateReader<MessageT>(
      channel_name, reader_func);
  if (reader != nullptr) {
    readers_.emplace(std::make_pair(channel_name, reader));
  }
  return reader;
}

template <typename Request, typename Response>
auto Node::CreateService(
    const std::string& service_name,
    const typename Service<Request, Response>::ServiceCallback&
        service_callback) -> std::shared_ptr<Service<Request, Response>> {
  return node_service_impl_->template CreateService<Request, Response>(
      service_name, service_callback);
}

template <typename Request, typename Response>
auto Node::CreateClient(const std::string& service_name)
    -> std::shared_ptr<Client<Request, Response>> {
  return node_service_impl_->template CreateClient<Request, Response>(
      service_name);
}

template <typename MessageT>
auto Node::GetReader(const std::string& name)
    -> std::shared_ptr<Reader<MessageT>> {
  std::lock_guard<std::mutex> lg(readers_mutex_);
  auto it = readers_.find(name);
  if (it != readers_.end()) {
    return std::dynamic_pointer_cast<Reader<MessageT>>(it->second);
  }
  return nullptr;
}

}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_NODE_NODE_H_
