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
#ifndef CYBER_CLASS_LOADER_CLASS_LOADER_H_
#define CYBER_CLASS_LOADER_CLASS_LOADER_H_

#include <algorithm>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "cyber/class_loader/class_loader_register_macro.h"

namespace apollo {
namespace cyber {
namespace class_loader {

/**
 *  for library load,createclass object
 *  类加载器: 加载动态库，并实例化对象
 */
class ClassLoader {
 public:
  explicit ClassLoader(const std::string& library_path);
  virtual ~ClassLoader();

  // 库是否已经加载
  bool IsLibraryLoaded();
  // 加载库
  bool LoadLibrary();
  // 卸载库
  int UnloadLibrary();
  // 获取库的路径
  const std::string GetLibraryPath() const;
  // 获取类的名称
  template <typename Base>
  std::vector<std::string> GetValidClassNames();
  // 实例化类对象
  template <typename Base>
  std::shared_ptr<Base> CreateClassObj(const std::string& class_name);
  // 类是否有效
  template <typename Base>
  bool IsClassValid(const std::string& class_name);

 private:
  // 当类删除
  template <typename Base>
  void OnClassObjDeleter(Base* obj);

 private:
  // 类的路径
  std::string library_path_;
  // 加载库引用次数
  int loadlib_ref_count_;
  // 加载库引用次数锁
  std::mutex loadlib_ref_count_mutex_;
  // 类对象引用次数
  int classobj_ref_count_;
  // 类对象引用次数锁
  std::mutex classobj_ref_count_mutex_;
};

template <typename Base>
std::vector<std::string> ClassLoader::GetValidClassNames() {
  return (utility::GetValidClassNames<Base>(this));
}

template <typename Base>
bool ClassLoader::IsClassValid(const std::string& class_name) {
  std::vector<std::string> valid_classes = GetValidClassNames<Base>();
  return (std::find(valid_classes.begin(), valid_classes.end(), class_name) !=
          valid_classes.end());
}

/* 实例化类对象 */
template <typename Base>
std::shared_ptr<Base> ClassLoader::CreateClassObj(
    const std::string& class_name) {
  // 加载动态库
  if (!IsLibraryLoaded()) {
    LoadLibrary();
  }

  // 根据类的名称创建对象
  Base* class_object = utility::CreateClassObj<Base>(class_name, this);
  if (class_object == nullptr) {
    AWARN << "CreateClassObj failed, ensure class has been registered. "
          << "classname: " << class_name << ",lib: " << GetLibraryPath();
    return std::shared_ptr<Base>();
  }

  // 创建类对象时引用计数加1
  std::lock_guard<std::mutex> lck(classobj_ref_count_mutex_);
  classobj_ref_count_ = classobj_ref_count_ + 1;
  // 指定类的析构函数
  std::shared_ptr<Base> classObjSharePtr(
      class_object, std::bind(&ClassLoader::OnClassObjDeleter<Base>, this,
                              std::placeholders::_1));
  return classObjSharePtr;
}

template <typename Base>
void ClassLoader::OnClassObjDeleter(Base* obj) {
  if (nullptr == obj) {
    return;
  }

  delete obj;
  // 删除类对象时引用计数减1
  std::lock_guard<std::mutex> lck(classobj_ref_count_mutex_);
  --classobj_ref_count_;
}

}  // namespace class_loader
}  // namespace cyber
}  // namespace apollo
#endif  // CYBER_CLASS_LOADER_CLASS_LOADER_H_
