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

#include "cyber/transport/common/identity.h"

#include <uuid/uuid.h>

#include "cyber/common/util.h"

namespace apollo {
namespace cyber {
namespace transport {

/* 构造函数 */
Identity::Identity(bool need_generate) : hash_value_(0) {
  std::memset(data_, 0, ID_SIZE);
  if (need_generate) {
    uuid_t uuid;
    uuid_generate(uuid);
    std::memcpy(data_, uuid, ID_SIZE);
    Update();
  }
}

/* 拷贝构造函数 */
Identity::Identity(const Identity& rhs) {
  std::memcpy(data_, rhs.data_, ID_SIZE);
  hash_value_ = rhs.hash_value_;
}

/* 析构函数 */
Identity::~Identity() {}

/* 重载拷贝赋值运算符 */
Identity& Identity::operator=(const Identity& rhs) {
  if (this != &rhs) {
    std::memcpy(data_, rhs.data_, ID_SIZE);
    hash_value_ = rhs.hash_value_;
  }
  return *this;
}

/* 重载等于运算符 */
bool Identity::operator==(const Identity& rhs) const {
  return std::memcmp(data_, rhs.data_, ID_SIZE) == 0;
}

/* 重载不等于运算符 */
bool Identity::operator!=(const Identity& rhs) const {
  return std::memcmp(data_, rhs.data_, ID_SIZE) != 0;
}

/* 转换为字符串 */
std::string Identity::ToString() const { return std::to_string(hash_value_); }

/* 获取长度 */
size_t Identity::Length() const { return ID_SIZE; }

/* 获取哈希值 */
uint64_t Identity::HashValue() const { return hash_value_; }

/* 更新 */
void Identity::Update() {
  hash_value_ = common::Hash(std::string(data_, ID_SIZE));
}

}  // namespace transport
}  // namespace cyber
}  // namespace apollo
