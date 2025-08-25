// 跨平台共享库入口函数实现
#ifdef _WIN32
// Windows平台DLL入口
#include <windows.h>
#include "CefHostCppWrapper.h"

// Windows DLL入口函数
BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        // DLL被进程加载时执行
        break;
    case DLL_PROCESS_DETACH:
        // DLL被进程卸载时执行
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        // 线程相关事件（可选处理）
        break;
    }
    return TRUE;
}

#else
// Linux/macOS平台初始化/清理函数
#include <unistd.h>

// 库加载时自动执行（类似DLL_PROCESS_ATTACH）
__attribute__((constructor)) void on_library_load() {
    // 库加载时的初始化逻辑
}

// 库卸载时自动执行（类似DLL_PROCESS_DETACH）
__attribute__((destructor)) void on_library_unload() {
    // 库卸载时的清理逻辑
}

#endif
