# lib_probuffer
probuffer


# 全局默认编码（可选，如默认按 UTF-8 处理）
add_compile_options("/execution-charset:utf-8")  # 字符串常量输出为 UTF-8

# --------------------------
# 处理 GB2312 编码的文件
# --------------------------
set(GB2312_FILES
    "src_gb2312/file_gb2312.cpp"
    # 其他 GB2312 文件...
)

# 为 GB2312 文件单独指定源编码为 936（GB2312）
set_source_files_properties(${GB2312_FILES}
    PROPERTIES
    COMPILE_OPTIONS "/source-charset:936"
)

# --------------------------
# 处理 UTF-8 编码的文件
# --------------------------
set(UTF8_FILES
    "src_utf8/file_utf8.cpp"
    # 其他 UTF-8 文件...
)

# 为 UTF-8 文件单独指定源编码为 utf-8
set_source_files_properties(${UTF8_FILES}
    PROPERTIES
    COMPILE_OPTIONS "/source-charset:utf-8"
)

# --------------------------
# 定义目标（包含所有文件）
# --------------------------
add_executable(my_project
    ${GB2312_FILES}
    ${UTF8_FILES}
)

