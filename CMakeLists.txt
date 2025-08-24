cmake_minimum_required (VERSION 3.16)
project (probuffer C CXX)

# 限制只生成Debug和Release两种配置（必须在添加目标前设置）
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "仅保留Debug和Release配置" FORCE)

# 设置默认安装路径前缀（可被命令行参数覆盖）
if(WIN32)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/bin" CACHE PATH "安装路径前缀" FORCE)
else()
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/bin" CACHE PATH "安装路径前缀" FORCE)
endif()

add_library(probuffer SHARED)
add_executable(probuffer_test)

file(GLOB_RECURSE SDK_FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/sdk/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/sdk/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/sdk/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/sdk/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/sdk/*.cc"
)

file(GLOB_RECURSE PROTOSRC_FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/protosrc/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/protosrc/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/protosrc/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/protosrc/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/protosrc/*.cc"
)

file(GLOB_RECURSE TEST_FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
)

target_sources(probuffer PRIVATE ${SDK_FILES} ${PROTOSRC_FILES})
target_sources(probuffer_test PRIVATE ${TEST_FILES})

target_include_directories(probuffer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/sdk")
target_include_directories(probuffer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/sdk/utf8_range")
target_include_directories(probuffer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/protosrc")
target_include_directories(probuffer_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# 预定义宏
target_compile_definitions(probuffer PRIVATE CEFHOST_C_API_EXPORTS)

set_property(TARGET probuffer probuffer_test PROPERTY CXX_STANDARD 20)  #指定C++标准 

# VS筛选器配置
# 简化的VS筛选器配置（CMake 3.8+支持）
if(MSVC)
  # 自动根据sdk目录结构创建筛选器，根筛选器名为"sdk"
  # PREFIX "sdk" 确保所有筛选器都以sdk为前缀（保持与源目录的对应关系）
  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/sdk" PREFIX "sdk" FILES ${SDK_FILES})
  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/protosrc" PREFIX "protosrc" FILES ${PROTOSRC_FILES})
  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "src" FILES ${TEST_FILES})
  
  # 设置静态链接CRT（仅MSVC有效）
  set_property(TARGET probuffer probuffer_test PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") #C
  set_property(TARGET probuffer probuffer_test PROPERTY CXX_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")  #C++
  
  # 设置VS的启动项目为probuffer_test
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT probuffer_test)
  
  set(DEF_FILE "${CMAKE_CURRENT_SOURCE_DIR}/protosrc/probuff.def")
  target_link_options(probuffer PRIVATE /DEF:${DEF_FILE})
  
  # 强制编译器将源文件视为 UTF-8 编码
  #target_compile_options(probuffer PRIVATE "/utf-8")
  #target_compile_options(probuffer_test PRIVATE "/utf-8")
endif()

# Linux平台，静态链接C/C++运行时库
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_options(probuffer probuffer_test PRIVATE -static-libgcc)
  target_link_options(probuffer probuffer_test PRIVATE -static-libstdc++)
endif()

# 设置输出基础目录
set(OUTPUT_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# 根据构建类型（Debug/Release）设置不同输出目录
set_target_properties(probuffer probuffer_test PROPERTIES
  # Windows DLL输出路径
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_BASE_DIR}/debug"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_BASE_DIR}/release"
  # Windows lib输出路径
  LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_BASE_DIR}/debug"
  LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_BASE_DIR}/release"
  # Linux/macOS 动态库输出路径（统一放在对应构建类型目录）
  ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_BASE_DIR}/debug"
  ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_BASE_DIR}/release"
)

# 先判断是否为 Release 模式
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  # --------------------------
  # 编译选项（区分平台）
  # --------------------------
  if(WIN32)  # Windows 平台（MSVC）
    target_compile_options(probuffer PRIVATE
      /Z7- /Zi- /Zc:inline-  # 禁用调试信息
      /Os                    # 最小化大小
      /GF                    # 字符串池化
    )
  elseif(UNIX)  # Linux/macOS 平台（GCC/Clang）
    target_compile_options(probuffer PRIVATE
      -g0                    # 禁用调试信息
      -Os                    # 最小化大小
	  -ffunction-sections    # 将每个函数打包为独立段（配合链接器移除未用函数）
	  -fdata-sections        # 将每个数据块打包为独立段（配合链接器移除未用数据）
      -fmerge-all-constants  # 合并相同常量（类似 MSVC 的 /GF）
    )
  endif()

  # --------------------------
  # 链接选项（区分平台）
  # --------------------------
  if(WIN32)  # Windows 平台（MSVC 链接器）
    target_link_options(probuffer PRIVATE
      /DEBUG:NO # 禁用链接器调试信息（对应VS“生成调试信息: 否”）
      /OPT:ICF  # 合并相同函数/数据（COMDAT折叠）
      /OPT:REF  # 移除未引用函数/数据
    )
  elseif(UNIX)  # Linux/macOS 平台（GCC/Clang 链接器）
    target_link_options(probuffer PRIVATE
      -s                     # 剥离符号表（禁用链接器调试信息，减小体积）
      -Wl,--gc-sections      # 移除未引用的段（配合编译选项 -ffunction-sections/-fdata-sections）
      -Wl,--icf=all          # 合并相同段（类似 MSVC 的 /OPT:ICF，GCC 4.5+ 支持）
    )
  endif()
endif()










