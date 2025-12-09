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
 * @class Node 节点
 * @brief Node is the fundamental building block of Cyber RT.
 * every module contains and communicates through the node.
 * 每个模块都包含一个node，并通过该node进行通信.
 * A module can have different types of communication by defining
 * read/write and/or service/client in a node.
 * 模块可以定义不同的通信方式：read/write 或 service/client.
 * @warning Duplicate name is not allowed in topo objects, such as node,
 * reader/writer, service/client in the topo.
 */
class Node {
 public:
  template <typename M0, typename M1, typename M2, typename M3>
  friend class Component;
  friend class TimerComponent;

  /* 初始化 */
  friend bool Init(const char*, const std::string&);

  /* 创建Node */
  friend std::unique_ptr<Node> CreateNode(const std::string&,
                                          const std::string&);
  
  /* 析构函数 */
  virtual ~Node();

  /**
   * @brief Get node's name.  获取Node的名称
   * @warning duplicate node name is not allowed in the topo.
   * 拓扑结构中的Node不允许重名
   */
  const std::string& Name() const;

  /**
   * @brief Create a Writer with specific message type.
   * 以特定Message类型创建Writer.
   *
   * @tparam MessageT Message Type.  Message类型
   * @param role_attr is a protobuf message RoleAttributes, which includes the
   * channel name and other info. RoleAttributes
   * @return std::shared_ptr<Writer<MessageT>> result Writer Object
   */
  template <typename MessageT>
  auto CreateWriter(const proto::RoleAttributes& role_attr)
      -> std::shared_ptr<Writer<MessageT>>;

  /**
   * @brief Create a Writer with specific message type.
   * 以特定Message类型创建Writer.
   *
   * @tparam MessageT Message Type.  Message类型
   * @param channel_name the channel name to be published.  Channel名称
   * @return std::shared_ptr<Writer<MessageT>> result Writer Object
   */
  template <typename MessageT>
  auto CreateWriter(const std::string& channel_name)
      -> std::shared_ptr<Writer<MessageT>>;

  /**
   * @brief Create a Reader with specific message type with channel name
   * qos and other configs used will be default.
   * 以特定Message类型、Channel名称和其他配置创建Reader.
   * @tparam MessageT Message Type.  Message类型
   * @param channel_name the channel of the reader subscribed.  Reader订阅的Channel名称.
   * @param reader_func invoked when message receive. 接收到Message时执行的回调函数.
   * @return std::shared_ptr<cyber::Reader<MessageT>> result Reader Object
   */
  template <typename MessageT>
  auto CreateReader(const std::string& channel_name,
                    const CallbackFunc<MessageT>& reader_func = nullptr)
      -> std::shared_ptr<cyber::Reader<MessageT>>;

  /**
   * @brief Create a Reader with specific message type with reader config
   * 以特定Message类型、Reader配置创建Reader.
   * @tparam MessageT Message Type. message 类型
   * @param config instance of `ReaderConfig`,  Reader配置，包括Channel名称、QoS、Pending队列大小等.
   * include channel name, qos and pending queue size
   * @param reader_func invoked when message receive  接收到Message时执行的回调函数.
   * @return std::shared_ptr<cyber::Reader<MessageT>> result Reader Object
   */
  template <typename MessageT>
  auto CreateReader(const ReaderConfig& config,
                    const CallbackFunc<MessageT>& reader_func = nullptr)
      -> std::shared_ptr<cyber::Reader<MessageT>>;

  /**
   * @brief Create a Reader object with `RoleAttributes`
   * 以RoleAttributes创建Reader.
   * @tparam MessageT Message Type  Message类型
   * @param role_attr instance of `RoleAttributes`, RoleAttributes实例
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
   * 以特定Serice名称创建Service对象.
   *
   * @tparam Request Message Type of the Request. Request消息类型
   * @tparam Response Message Type of the Response. Response消息类型
   * @param service_name specific service name to a serve. Service名称
   * @param service_callback invoked when a service is called. Service被调用时执行的回调函数.
   * @return std::shared_ptr<Service<Request, Response>> result `Service`
   */
  template <typename Request, typename Response>
  auto CreateService(const std::string& service_name,
                     const typename Service<Request, Response>::ServiceCallback&
                         service_callback)
      -> std::shared_ptr<Service<Request, Response>>;

  /**
   * @brief Create a Client object to request Service with `service_name`
   * 创建Client对象并请求Service服务
   * @tparam Request Message Type of the Request. Request消息类型
   * @tparam Response Message Type of the Response. Response消息类型
   * @param service_name specific service name to a Service. Service名称
   * @return std::shared_ptr<Client<Request, Response>> result `Client`
   */
  template <typename Request, typename Response>
  auto CreateClient(const std::string& service_name)
      -> std::shared_ptr<Client<Request, Response>>;

  /* Delete Reader */
  bool DeleteReader(const std::string& channel_name);
  bool DeleteReader(const ReaderConfig& config);
  bool DeleteReader(const proto::RoleAttributes& role_attr);
  /**
   * @brief Observe all readers' data. 观察所有Reader的数据.
   */
  void Observe();

  /**
   * @brief clear all readers' data. 清除所有Reader的数据.
   */
  void ClearData();

  /**
   * @brief Get the Reader object that subscribe `channel_name`
   * 获取订阅Channel的Reader对象.
   * @tparam MessageT Message Type. 消息类型.
   * @param channel_name channel name. Channel名称.
   * @return std::shared_ptr<Reader<MessageT>> result reader
   */
  template <typename MessageT>
  auto GetReader(const std::string& channel_name)
      -> std::shared_ptr<Reader<MessageT>>;

 private:
  /* 构造函数 */
  explicit Node(const std::string& node_name,
                const std::string& name_space = "");

  /* Node名称 */
  std::string node_name_;
  /* 命名空间 */
  std::string name_space_;

  /* Reader互斥锁 */
  std::mutex readers_mutex_;
  /* Reader集合<ChannelName, Reader>*/
  std::map<std::string, std::shared_ptr<ReaderBase>> readers_;

  /* NodeChannel实现 */
  std::unique_ptr<NodeChannelImpl> node_channel_impl_ =
      nullptr;
  /* NodeService实现 */
  std::unique_ptr<NodeServiceImpl> node_service_impl_ =
      nullptr;
};

template <typename MessageT>
auto Node::CreateWriter(const proto::RoleAttributes& role_attr)
    -> std::shared_ptr<Writer<MessageT>> {
  /* template关键字显式指定模板参数*/
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

  /* 创建Reader */
  auto reader = node_channel_impl_->template CreateReader<MessageT>(
      role_attr, reader_func);

  /* 更新Reader集合 */
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

  /* 判断重名 */
  if (readers_.find(config.channel_name) != readers_.end()) {
    AWARN << "Failed to create reader: reader with the same channel already "
             "exists.";
    return nullptr;
  }

  /* 创建Reader */
  auto reader =
      node_channel_impl_->template CreateReader<MessageT>(config, reader_func);

  /* 更新Reader集合 */
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

  /* 判断重名 */
  if (readers_.find(channel_name) != readers_.end()) {
    AWARN << "Failed to create reader: reader with the same channel already "
             "exists.";
    return nullptr;
  }

  /* 创建Reader */
  auto reader = node_channel_impl_->template CreateReader<MessageT>(
      channel_name, reader_func);

  /* 更新Reader集合 */
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

  /* 获取订阅Channel的Reader对象 */
  auto it = readers_.find(name);
  if (it != readers_.end()) {
    return std::dynamic_pointer_cast<Reader<MessageT>>(it->second);
  }
  return nullptr;
}

}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_NODE_NODE_H_
