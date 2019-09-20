# src/scene/CMakeLists.txt - Dream CEF CMake file
# TODO: Add the patches / add all of the chromium shit

message("dos/src/dreamcef/cmakelists.txt")

file(GLOB SOURCES 
  "*.cpp"
  "*.tpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

# Add CEF binaries 
link_directories(${CMAKE_BINARY_DIR})

add_executable(dreamcef
    ${SOURCES}
)


if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # Windows Subsystem vs Console
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

    message("flags: ${CMAKE_CXX_FLAGS}")
    message("debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
    message("release flags: ${CMAKE_CXX_FLAGS_RELEASE}")
endif()

target_link_libraries(dreamcef
    debug ${CMAKE_SOURCE_DIR}/src/third_party/CEF/CEF_BINARY_3538/Debug/libcef_dll_wrapper.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/CEF/CEF_BINARY_3538/Debug/libcef.dll.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/libcurl/curl-7.49.1/lib/Windows/x64/Debug/libcurl_imp.lib
    
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/CEF/CEF_BINARY_3538/OfficialBuild/libcef_dll_wrapper.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/CEF/CEF_BINARY_3538/OfficialBuild/libcef.dll.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/libcurl/curl-7.49.1/lib/Windows/x64/Release/libcurl_imp.lib
    
    ${CMAKE_SOURCE_DIR}/src/third_party/openssl/openssl-1.0.2/x64/Release/libssl.lib
    ${CMAKE_SOURCE_DIR}/src/third_party/openssl/openssl-1.0.2/x64/Release/libcrypto.lib

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

# Not sure if these are working
# set_property(TARGET dreamcef PROPERTY
#              MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
# 
# set_property(TARGET dreamcef PROPERTY
#              MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

