# src/junk/vulkaninitproj/CMakeLists.txt - Init Vulkan Proj

file(GLOB SOURCES 
  "*.cpp"
  "*.tpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

# Add CEF binaries 
link_directories(${CMAKE_BINARY_DIR})

add_executable(vulkaninitproj
    ${SOURCES}
        ../helloworld/main.cpp)

target_link_libraries(vulkaninitproj
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
