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

#ifndef CYBER_TIME_TIME_H_
#define CYBER_TIME_TIME_H_

#include <limits>
#include <string>

#include "cyber/time/duration.h"

namespace apollo {
namespace cyber {

/**
 * @brief Cyber has builtin time type Time.
 *        时间类型
 */
class Time {
 public:
  /* MAX / MIN */
  static const Time MAX;
  static const Time MIN;

  /* 构造函数 */
  Time() = default;
  explicit Time(uint64_t nanoseconds);
  explicit Time(int nanoseconds);
  explicit Time(double seconds);
  Time(uint32_t seconds, uint32_t nanoseconds);
  /* 拷贝构造函数 */
  Time(const Time& other);
  /* 拷贝赋值运算符 */
  Time& operator=(const Time& other);

  /**
   * @brief get the current time.
   * 获取当前时间
   * @return return the current time.
   */
  static Time Now();
  static Time MonoTime();

  /**
   * @brief Sleep Until time.
   * 休眠到指定时间
   * @param time the Time object.
   */
  static void SleepUntil(const Time& time);

  /**
   * @brief convert time to second.
   * 时间转换为秒(s)
   * @return return a double value unit is second.
   */
  double ToSecond() const;

  /**
   * @brief convert time to microsecond (us).
   * 时间转换为微秒(us)
   * @return return a unit64_t value unit is us.
   */
  uint64_t ToMicrosecond() const;

  /**
   * @brief convert time to nanosecond.
   * 时间转换为纳秒(ns)
   * @return return a unit64_t value unit is nanosecond.
   */
  uint64_t ToNanosecond() const;

  /**
   * @brief convert time to a string.
   * 时间转换为字符串
   * @return return a string.
   */
  std::string ToString() const;

  /**
   * @brief determine if time is 0
   * 判断时间是否为0
   * @return return true if time is 0
   */
  bool IsZero() const;

  /* 重载运算符 */
  Duration operator-(const Time& rhs) const;
  Time operator+(const Duration& rhs) const;
  Time operator-(const Duration& rhs) const;
  Time& operator+=(const Duration& rhs);
  Time& operator-=(const Duration& rhs);
  bool operator==(const Time& rhs) const;
  bool operator!=(const Time& rhs) const;
  bool operator>(const Time& rhs) const;
  bool operator<(const Time& rhs) const;
  bool operator>=(const Time& rhs) const;
  bool operator<=(const Time& rhs) const;

 private:
  /* 时间值(ns) */
  uint64_t nanoseconds_ = 0;
};

/* 重载<<运算符 */
std::ostream& operator<<(std::ostream& os, const Time& rhs);

}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_TIME_TIME_H_
