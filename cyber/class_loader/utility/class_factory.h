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
#ifndef CYBER_CLASS_LOADER_UTILITY_CLASS_FACTORY_H_
#define CYBER_CLASS_LOADER_UTILITY_CLASS_FACTORY_H_

#include <string>
#include <typeinfo>
#include <vector>

namespace apollo {
namespace cyber {
namespace class_loader {

class ClassLoader;

namespace utility {

/* AbstractClassFactoryBase */
class AbstractClassFactoryBase {
 public:
  /* 构造函数 */
  AbstractClassFactoryBase(const std::string& class_name,
                           const std::string& base_class_name);
  /* 析构函数 */
  virtual ~AbstractClassFactoryBase();

  /* 设置动态库的相对路径 */
  void SetRelativeLibraryPath(const std::string& library_path);
  /* 添加ClassLoader */
  void AddOwnedClassLoader(ClassLoader* loader);
  /* 删除ClassLoader */
  void RemoveOwnedClassLoader(const ClassLoader* loader);
  /* ClassLoader是否属于该ClassFactory */
  bool IsOwnedBy(const ClassLoader* loader);
  bool IsOwnedByAnybody();
  /* 获取所有ClassLoader */
  std::vector<ClassLoader*> GetRelativeClassLoaders();
  /* 获取相对动态库路径 */
  const std::string GetRelativeLibraryPath() const;
  /* 获取基类名称 */
  const std::string GetBaseClassName() const;
  /* 获取类名称 */
  const std::string GetClassName() const;

 protected:
  std::vector<ClassLoader*> relative_class_loaders_;
  std::string relative_library_path_; /* 相对动态库路径 */
  std::string base_class_name_; /* 基类名称 */
  std::string class_name_;  /* 类名称 */
};

/* 模板类AbstractClassFactory->AbstractClassFactoryBase */
template <typename Base>
class AbstractClassFactory : public AbstractClassFactoryBase {
 public:
  AbstractClassFactory(const std::string& class_name,
                       const std::string& base_class_name)
      : AbstractClassFactoryBase(class_name, base_class_name) {}

  /* 创建对象 */
  virtual Base* CreateObj() const = 0;

 private:
  /* 构造函数*/
  AbstractClassFactory();
  /* 拷贝构造函数 */
  AbstractClassFactory(const AbstractClassFactory&);
  /* 拷贝赋值运算符 */
  AbstractClassFactory& operator=(const AbstractClassFactory&);
};

/* 模板类ClassFactory->AbstractClassFactory */
template <typename ClassObject, typename Base>
class ClassFactory : public AbstractClassFactory<Base> {
 public:
  /* 构造函数 */
  ClassFactory(const std::string& class_name,
               const std::string& base_class_name)
      : AbstractClassFactory<Base>(class_name, base_class_name) {}

  /* 创建对象 */
  Base* CreateObj() const { return new ClassObject; }
};

}  // namespace utility
}  // namespace class_loader
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_CLASS_LOADER_UTILITY_CLASS_FACTORY_H_
