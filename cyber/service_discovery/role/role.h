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

#ifndef CYBER_SERVICE_DISCOVERY_ROLE_ROLE_H_
#define CYBER_SERVICE_DISCOVERY_ROLE_ROLE_H_

#include <cstdint>
#include <memory>
#include <string>

#include "cyber/proto/role_attributes.pb.h"

namespace apollo {
namespace cyber {
namespace service_discovery {

class RoleBase;
using RolePtr = std::shared_ptr<RoleBase>;

using RoleNode = RoleBase;
using RoleNodePtr = std::shared_ptr<RoleNode>;

class RoleWriter;
using RoleWriterPtr = std::shared_ptr<RoleWriter>;
using RoleReader = RoleWriter;
using RoleReaderPtr = std::shared_ptr<RoleReader>;

class RoleServer;
using RoleServerPtr = std::shared_ptr<RoleServer>;
using RoleClient = RoleServer;
using RoleClientPtr = std::shared_ptr<RoleClient>;

/* Role 基类 */
class RoleBase {
 public:
  /* 默认构造函数 */
  RoleBase();
  explicit RoleBase(const proto::RoleAttributes& attr,
                    uint64_t timestamp_ns = 0);
  /* 析构函数 */
  virtual ~RoleBase() = default;

  /* 匹配 */
  virtual bool Match(const proto::RoleAttributes& target_attr) const;
  /* 比较是否早于 */
  bool IsEarlierThan(const RoleBase& other) const;

  /* 获取属性 */
  const proto::RoleAttributes& attributes() const { return attributes_; }
  /* 设置属性 */
  void set_attributes(const proto::RoleAttributes& attr) { attributes_ = attr; }

  /* 获取时间戳 */
  uint64_t timestamp_ns() const { return timestamp_ns_; }
  /* 设置时间戳 */
  void set_timestamp_ns(uint64_t timestamp_ns) { timestamp_ns_ = timestamp_ns; }

 protected:
  /* 属性 */
  proto::RoleAttributes attributes_;
  /* 时间戳 */
  uint64_t timestamp_ns_;
};

/* Role Writer */
class RoleWriter : public RoleBase {
 public:
  /* 构造函数 */
  RoleWriter() {}
  /* 构造函数 */
  explicit RoleWriter(const proto::RoleAttributes& attr,
                      uint64_t timestamp_ns = 0);
  /* 析构函数 */
  virtual ~RoleWriter() = default;

  /* 匹配 */
  bool Match(const proto::RoleAttributes& target_attr) const override;
};

/* Role Server */
class RoleServer : public RoleBase {
 public:
  /* 构造函数 */
  RoleServer() {}
  /* 构造函数 */
  explicit RoleServer(const proto::RoleAttributes& attr,
                      uint64_t timestamp_ns = 0);
  /* 析构函数 */
  virtual ~RoleServer() = default;

  /* 匹配 */
  bool Match(const proto::RoleAttributes& target_attr) const override;
};

}  // namespace service_discovery
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_SERVICE_DISCOVERY_ROLE_ROLE_H_
