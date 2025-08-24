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
  
  target_link_options(probuffer PRIVATE /DEF:"${CMAKE_CURRENT_SOURCE_DIR}/protosrc/protobuf.def")
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












