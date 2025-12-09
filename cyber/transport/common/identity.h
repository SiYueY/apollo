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

#ifndef CYBER_TRANSPORT_COMMON_IDENTITY_H_
#define CYBER_TRANSPORT_COMMON_IDENTITY_H_

#include <cstdint>
#include <cstring>
#include <string>

namespace apollo {
namespace cyber {
namespace transport {

constexpr uint8_t ID_SIZE = 8;

/* 标识 Identity */
class Identity {
 public:
   /* 构造函数 */
  explicit Identity(bool need_generate = true);
  Identity(const Identity& another);
  /* 析构函数 */
  virtual ~Identity();

  /* 重载拷贝赋值运算符 */
  Identity& operator=(const Identity& another);
  /* 重载 == 运算符 */
  bool operator==(const Identity& another) const;
  /* 重载 !=运算符 */
  bool operator!=(const Identity& another) const;

  /* 转换为字符串 */
  std::string ToString() const;
  /* 获取长度 */
  size_t Length() const;
  /* 获取哈希值 */
  uint64_t HashValue() const;

  /* 获取数据 */
  const char* data() const { return data_; }
  /* 设置数据 */
  void set_data(const char* data) {
    if (data == nullptr) {
      return;
    }
    std::memcpy(data_, data, sizeof(data_));
    Update();
  }

 private:
  /* 更新 */
  void Update();

  /* 数据 */
  char data_[ID_SIZE];
  /* 哈希值 */
  uint64_t hash_value_;
};

}  // namespace transport
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_TRANSPORT_COMMON_IDENTITY_H_
