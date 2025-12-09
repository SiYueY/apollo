/******************************************************************************
 * Copyright 2020 The Apollo Authors. All Rights Reserved.
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
//
// Adapted from poco/Foundation/include/Poco/SharedLibrary.h
//
// Definition of the SharedLibrary class.
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:     BSL-1.0
//

#ifndef CYBER_CLASS_LOADER_SHARED_LIBRARY_SHARED_LIBRARY_H_
#define CYBER_CLASS_LOADER_SHARED_LIBRARY_SHARED_LIBRARY_H_

#include <mutex>
#include <string>

#include "cyber/class_loader/shared_library/exceptions.h"

namespace apollo {
namespace cyber {
namespace class_loader {

// The SharedLibrary class dynamically loads shared libraries at run-time.
/* SharedLibrary类在运行时动态加载共享库 */
class SharedLibrary {
 public:
  enum Flags {
    // On platforms that use dlopen(), use RTLD_GLOBAL. This is the default
    // if no flags are given.
    SHLIB_GLOBAL = 1,

    // On platforms that use dlopen(), use RTLD_LOCAL instead of RTLD_GLOBAL.
    //
    // Note that if this flag is specified, RTTI (including dynamic_cast and
    // throw) will not work for types defined in the shared library with GCC
    // and possibly other compilers as well. See
    // http://gcc.gnu.org/faq.html#dso for more information.
    SHLIB_LOCAL = 2,
  };

  // Creates a SharedLibrary object.
  /* 默认构造函数: 创建SharedLibrary对象 */
  SharedLibrary() = default;

  // Destroys the SharedLibrary. The actual library
  // remains loaded.
  /* 析构函数: 销毁SharedLibrary对象, 实际的库仍然被加载 */
  virtual ~SharedLibrary();

  // Creates a SharedLibrary object and loads a library
  // from the given path.
  /* 构造函数: 创建SharedLibrary对象并从给定的路径加载库 */
  explicit SharedLibrary(const std::string& path);

  // Creates a SharedLibrary object and loads a library
  // from the given path, using the given flags.
  // See the Flags enumeration for valid values.
  /* 构造函数: 创建SharedLibrary对象并从给定的路径加载库, 使用给定的flags */
  SharedLibrary(const std::string& path, int flags);

 public:
  // Loads a shared library from the given path.
  // Throws a LibraryAlreadyLoadedException if
  // a library has already been loaded.
  // Throws a LibraryLoadException if the library
  // cannot be loaded.
  /* 加载共享库: 从给定的路径加载共享库. 
     若已加载库, 抛出LibraryAlreadyLoadedException异常. 
     若加载失败, 抛出LibraryLoadException异常. */
  void Load(const std::string& path);

  // Loads a shared library from the given path,
  // using the given flags. See the Flags enumeration
  // for valid values.
  // Throws a LibraryAlreadyLoadedException if
  // a library has already been loaded.
  // Throws a LibraryLoadException if the library
  // cannot be loaded.
  /* 加载共享库: 从给定的路径加载共享库, 使用给定的flags. 
     若已加载库, 抛出LibraryAlreadyLoadedException异常. 
     若加载失败, 抛出LibraryLoadException异常. */
  void Load(const std::string& path, int flags);

  // Unloads a shared library.
  /* 卸载共享库: 卸载共享库 */
  void Unload();

  // Returns true iff a library has been loaded.
  /* 是否已加载库: 若已加载库, 返回true; 否则返回false. */
  bool IsLoaded();

  // Returns true iff the loaded library contains
  // a symbol with the given name.
  /* 是否存在符号: 若已加载库且包含给定名称的符号, 返回true; 否则返回false. */
  bool HasSymbol(const std::string& name);

  // Returns the address of the symbol with
  // the given name. For functions, this
  // is the entry point of the function.
  // Throws a SymbolNotFoundException if the
  // symbol does not exist.
  /* 获取给定名称的符号地址。对于函数即函数的入口地址. 
     若符号不存在, 抛出SymbolNotFoundException异常. */
  void* GetSymbol(const std::string& name);

  // Returns the path of the library, as specified in a call
  // to load() or the constructor.
  /* 获取库的路径: 获取load()或构造函数中指定的库路径. */
  inline const std::string& GetPath() const { return path_; }

 public:
  /* 禁用拷贝构造函数和拷贝赋值运算符 */
  SharedLibrary(const SharedLibrary&) = delete;
  SharedLibrary& operator=(const SharedLibrary&) = delete;

 private:
  /* 句柄 */
  void* handle_ = nullptr;
  /* 路径 */
  std::string path_;
  /* 互斥锁 */
  static std::mutex mutex_;
};

}  // namespace class_loader
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_CLASS_LOADER_SHARED_LIBRARY_SHARED_LIBRARY_H_
