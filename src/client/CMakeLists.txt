# src/client/CMakeLists.txt - Dream Client CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

# TODO: This should not be here!!!!!
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/webrtc/include/webrtc)
add_definitions(-DWEBRTC_WIN)

# Force Include the src/core/config.h
if(MSVC)
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/src/core/config.h")
else()
    # GCC or Clang
    add_definitions(-include ${CMAKE_SOURCE_DIR}/src/core/config.h)
endif()

# Multithreaded 
set(CMAKE_CXX_FLAGS_RELEASE "/MT")
set(CMAKE_CXX_FLAGS_DEBUG "/MTd")

add_executable(dreamclient
    ${SOURCES}
)

target_link_libraries(dreamclient
    debug ${CMAKE_SOURCE_DIR}/src/third_party/CEF/CEF_BINARY_3538/Debug/libcef_dll_wrapper.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/CEF/CEF_BINARY_3538/Debug/libcef.dll.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/libcurl/curl-7.49.1/lib/Windows/x64/Debug/libcurl_imp.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/openssl/openssl-1.0.2/x64/Debug/libssl.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/openssl/openssl-1.0.2/x64/Debug/libcrypto.lib
    #DreamOS.lib

    optimized ${CMAKE_SOURCE_DIR}/src/third_party/CEF/CEF_BINARY_3538/OfficialBuild/libcef_dll_wrapper.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/CEF/CEF_BINARY_3538/OfficialBuild/libcef.dll.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/libcurl/curl-7.49.1/lib/Windows/x64/Release/libcurl_imp.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/openssl/openssl-1.0.2/x64/Release/libssl.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/openssl/openssl-1.0.2/x64/Release/libcrypto.lib
    #DreamOS.lib

    # TODO: Make this into a windows dependent list 
    wininet.lib
    dnsapi.lib
    version.lib
    msimg32.lib
    ws2_32.lib
    usp10.lib
    psapi.lib
    dbghelp.lib
    winmm.lib
    shlwapi.lib
    kernel32.lib
    gdi32.lib
    winspool.lib
    comdlg32.lib
    advapi32.lib
    shell32.lib
    ole32.lib
    oleaut32.lib
    user32.lib
    uuid.lib
    odbc32.lib
    odbccp32.lib
    delayimp.lib
    credui.lib
    dmoguids.lib
    wmcodecdspuuid.lib
    amstrmid.lib
    msdmo.lib
    crypt32.lib
    iphlpapi.lib
    secur32.lib
    Strmiids.lib
)

