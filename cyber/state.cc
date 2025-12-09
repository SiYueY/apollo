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

#include "cyber/state.h"

#include <atomic>

namespace apollo {
namespace cyber {

namespace {
// 全局的原子变量 g_cyber_state 存储Cyber框架状态, 以确保在多线程环境下的安全访问
std::atomic<State> g_cyber_state;
}

/* 获取状态 */
State GetCyberState() { return g_cyber_state.load(); }

/* 设置状态 */
void SetState(const State& state) { g_cyber_state.store(state); }

}  // namespace cyber
}  // namespace apollo
