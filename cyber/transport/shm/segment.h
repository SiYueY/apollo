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

#ifndef CYBER_TRANSPORT_SHM_SEGMENT_H_
#define CYBER_TRANSPORT_SHM_SEGMENT_H_

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "cyber/transport/shm/block.h"
#include "cyber/transport/shm/shm_conf.h"
#include "cyber/transport/shm/state.h"

namespace apollo {
namespace cyber {
namespace transport {

class Segment;
using SegmentPtr = std::shared_ptr<Segment>;

/* 写Block */
struct WritableBlock {
  /* 索引 */
  uint32_t index = 0;
  /* Block */
  Block* block = nullptr;
  /* Buffer */
  uint8_t* buf = nullptr;
};

/* 读Block */
using ReadableBlock = WritableBlock;

/* 段 */
class Segment {
 public:
  /* 构造函数 */
  explicit Segment(uint64_t channel_id);
  /* 析构函数 */
  virtual ~Segment() {}

  /* 请求写入数据块 */
  bool AcquireBlockToWrite(std::size_t msg_size, WritableBlock* writable_block);
  /* 释放已写数据块 */
  void ReleaseWrittenBlock(const WritableBlock& writable_block);

  bool AcquireArenaBlockToWrite(
    std::size_t msg_size, WritableBlock* writable_block);
  void ReleaseArenaWrittenBlock(const WritableBlock& writable_block);

  bool AcquireBlockToRead(ReadableBlock* readable_block);
  /* 释放已读数据块 */
  void ReleaseReadBlock(const ReadableBlock& readable_block);

  bool AcquireArenaBlockToRead(ReadableBlock* readable_block);
  void ReleaseArenaReadBlock(const ReadableBlock& readable_block);

  bool InitOnly(uint64_t message_size);
  void* GetManagedShm();
  bool LockBlockForWriteByIndex(uint64_t block_index);
  bool ReleaseBlockForWriteByIndex(uint64_t block_index);
  bool LockBlockForReadByIndex(uint64_t block_index);
  bool ReleaseBlockForReadByIndex(uint64_t block_index);

  bool LockArenaBlockForWriteByIndex(uint64_t block_index);
  bool ReleaseArenaBlockForWriteByIndex(uint64_t block_index);
  bool LockArenaBlockForReadByIndex(uint64_t block_index);
  bool ReleaseArenaBlockForReadByIndex(uint64_t block_index);

 protected:
  /* 销毁 */
  virtual bool Destroy();
  /* 重置 */
  virtual void Reset() = 0;
  /* 移除 */
  virtual bool Remove() = 0;
  /* 打开 */
  virtual bool OpenOnly() = 0;
  /* 创建 */
  virtual bool OpenOrCreate() = 0;

  bool init_;
  /* 共享内存配置 */
  ShmConf conf_;
  uint64_t channel_id_;

  /* State */
  State* state_;
  /* Block */
  Block* blocks_;
  Block* arena_blocks_;
  void* managed_shm_;
  /* Block_Buf互斥锁 */
  std::mutex block_buf_lock_;
  std::mutex arena_block_buf_lock_;
  std::unordered_map<uint32_t, uint8_t*> block_buf_addrs_;
  std::unordered_map<uint32_t, uint8_t*> arena_block_buf_addrs_;

 private:
  /* 重新映射 */
  bool Remap();
  /* 重新创建 */
  bool Recreate(const uint64_t& msg_size);
  /* 获取下一可写的Block索引 */
  uint32_t GetNextWritableBlockIndex();
  uint32_t GetNextArenaWritableBlockIndex();
};

}  // namespace transport
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_TRANSPORT_SHM_SEGMENT_H_
