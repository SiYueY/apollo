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
#ifndef CYBER_MAINBOARD_MODULE_CONTROLLER_H_
#define CYBER_MAINBOARD_MODULE_CONTROLLER_H_

#include <memory>
#include <string>
#include <vector>

#include "cyber/proto/dag_conf.pb.h"

#include "cyber/class_loader/class_loader_manager.h"
#include "cyber/component/component.h"
#include "cyber/mainboard/module_argument.h"

namespace apollo {
namespace cyber {
namespace mainboard {

using apollo::cyber::proto::DagConfig;

/* 模块控制 */
class ModuleController {
 public:
  /* 构造函数 */
  explicit ModuleController(const ModuleArgument& args);
  /* 析构函数 */
  virtual ~ModuleController() = default;

  /* 初始化 */
  bool Init();
  /* 加载所有模块 */
  bool LoadAll();
  /* 清除所有模块 */
  void Clear();

 private:
  /* 加载模块 */
  bool LoadModule(const std::string& path);
  bool LoadModule(const DagConfig& dag_config);
  /* 获取组件数量 */
  int GetComponentNum(const std::string& path);

  int total_component_nums = 0; /* 组件总数 */
  bool has_timer_component = false; /* 是否存在定时器组件 */

  ModuleArgument args_; /* 模块参数 */
  class_loader::ClassLoaderManager class_loader_manager_; /* 类加载管理器 */
  std::vector<std::shared_ptr<ComponentBase>> component_list_;  /* 组件列表 */
};

/* 构造函数 */
inline ModuleController::ModuleController(const ModuleArgument& args)
    : args_(args) {}

/* 初始化 */
inline bool ModuleController::Init() { return LoadAll(); }

}  // namespace mainboard
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_MAINBOARD_MODULE_CONTROLLER_H_
