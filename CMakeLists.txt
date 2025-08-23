cmake_minimum_required (VERSION 3.16)
project (probuffer C CXX)

add_library(probuffer SHARED)
file(GLOB_RECURSE SDK_FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/sdk/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/sdk/*.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/sdk/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/sdk/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/sdk/*.cc"
)
target_sources(probuffer PRIVATE ${SDK_FILES})
target_include_directories(probuffer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/sdk/internal")
set_property(TARGET probuffer PROPERTY CXX_STANDARD 20)  #指定C++标准 

# VS筛选器配置
# 简化的VS筛选器配置（CMake 3.8+支持）
if(MSVC)
  # 自动根据sdk目录结构创建筛选器，根筛选器名为"sdk"
  # PREFIX "sdk" 确保所有筛选器都以sdk为前缀（保持与源目录的对应关系）
  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/sdk" PREFIX "sdk" FILES ${SDK_FILES})
  
  # 设置静态链接CRT（仅MSVC有效）
  set_property(TARGET probuffer PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") #C
  set_property(TARGET probuffer PROPERTY CXX_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")  #C++
endif()

# Linux平台，静态链接C/C++运行时库
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_options(probuffer PRIVATE -static-libgcc)
  target_link_options(probuffer PRIVATE -static-libstdc++)
endif()

# 设置输出基础目录
set(OUTPUT_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# 根据构建类型（Debug/Release）设置不同输出目录
set_target_properties(probuffer PROPERTIES
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












