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

#ifndef CYBER_COMMON_GLOBAL_DATA_H_
#define CYBER_COMMON_GLOBAL_DATA_H_

#include <string>
#include <unordered_map>

#include "cyber/proto/cyber_conf.pb.h"
#include "cyber/proto/transport_conf.pb.h"

#include "cyber/base/atomic_hash_map.h"
#include "cyber/base/atomic_rw_lock.h"
#include "cyber/common/log.h"
#include "cyber/common/macros.h"
#include "cyber/common/util.h"

namespace apollo {
namespace cyber {
namespace common {

using ::apollo::cyber::base::AtomicHashMap;
using ::apollo::cyber::proto::ClockMode;
using ::apollo::cyber::proto::CyberConfig;
using ::apollo::cyber::proto::RunMode;

/* 全局数据 */
class GlobalData {
 public:
  /* 析构函数 */
  ~GlobalData();

  /* 进程ID */
  int ProcessId() const;

  /* 设置/获取进程组 */
  void SetProcessGroup(const std::string& process_group);
  const std::string& ProcessGroup() const;

  /* 设置/获取组件数量 */
  void SetComponentNums(const int component_nums);
  int ComponentNums() const;

  /* 设置/获取调度策略 */
  void SetSchedName(const std::string& sched_name);
  const std::string& SchedName() const;

  /* Host IP */
  const std::string& HostIp() const;

  /* Host Name */
  const std::string& HostName() const;

  /* Cyber配置 */
  const CyberConfig& Config() const;

  /* 启用/禁用仿真模式 */
  void EnableSimulationMode();
  void DisableSimulationMode();

  /* 是否为真实模式 */
  bool IsRealityMode() const;

  /* 是否为模拟时间模式 */
  bool IsMockTimeMode() const;

  /* Channel是否启用共享内存 */
  bool IsChannelEnableArenaShm(std::string channel_name) const;
  bool IsChannelEnableArenaShm(uint64_t channel_id) const;
  apollo::cyber::proto::ArenaChannelConf GetChannelArenaConf(
      std::string channel_name) const&;
  apollo::cyber::proto::ArenaChannelConf GetChannelArenaConf(
      uint64_t channel_id) const&;

  /* 生成 Hash ID */
  static uint64_t GenerateHashId(const std::string& name) {
    return common::Hash(name);
  }

  /* 注册/获取节点 */
  static uint64_t RegisterNode(const std::string& node_name);
  static std::string GetNodeById(uint64_t id);

  /* 注册/获取Channel */
  static uint64_t RegisterChannel(const std::string& channel);
  static std::string GetChannelById(uint64_t id);

  /* 注册/获取Service */
  static uint64_t RegisterService(const std::string& service);
  static std::string GetServiceById(uint64_t id);

  /* 注册/获取TaskName */
  static uint64_t RegisterTaskName(const std::string& task_name);
  static std::string GetTaskNameById(uint64_t id);

 private:
  /* 初始化Host信息 */
  void InitHostInfo();
  /* 初始化配置 */
  bool InitConfig();

  // global config 全局Cyber配置
  CyberConfig config_;

  // host info Host信息
  std::string host_ip_;
  std::string host_name_;

  // process info 进程信息
  int process_id_;
  std::string process_group_;

  // component nums 组件数量
  int component_nums_ = 0;

  // sched policy info 调度策略信息
  std::string sched_name_ = "CYBER_DEFAULT";

  // run mode 运行模式
  RunMode run_mode_;
  // clock mode 时钟模式
  ClockMode clock_mode_;

  static AtomicHashMap<uint64_t, std::string, 512> node_id_map_;
  static AtomicHashMap<uint64_t, std::string, 256> channel_id_map_;
  static AtomicHashMap<uint64_t, std::string, 256> service_id_map_;
  static AtomicHashMap<uint64_t, std::string, 256> task_id_map_;

  DECLARE_SINGLETON(GlobalData)
};

}  // namespace common
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_COMMON_GLOBAL_DATA_H_
