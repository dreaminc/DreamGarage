# src/testclient/CMakeLists.txt - Dream Test Client Libraries CMake file

file(GLOB SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${SOURCES})

# TODO: This should not be here!!!!!
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/webrtc/include/webrtc)
add_definitions(-DWEBRTC_WIN)

# For OpenVR Matrix
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/openvr/headers)

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

# Windows Subsystem vs Console
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

add_executable(testclient
    ${SOURCES}
)

target_link_libraries(testclient core)
target_link_libraries(testclient os)
target_link_libraries(testclient sandbox)
target_link_libraries(testclient test)
target_link_libraries(testclient logger)
target_link_libraries(testclient modules)
target_link_libraries(testclient sense)
target_link_libraries(testclient cloud)
target_link_libraries(testclient ui)
target_link_libraries(testclient sound)
target_link_libraries(testclient pipeline)
target_link_libraries(testclient scene)
target_link_libraries(testclient hal)
target_link_libraries(testclient garage)
target_link_libraries(testclient hmd)
target_link_libraries(testclient webbrowser)

# Apps TODO: should do this based on what tests are being built
target_link_libraries(testclient 
    dreampeerapp
    dreamuserapp
    dreamshareviewapp
    dreambrowserapp
    dreamcontentviewapp
    dreamuibarapp
    dream2dmouseapp
    dreamgamepadcameraapp
    dreamvcamapp
    dreamusercontrolareaapp
    dreamenvironmentapp
    dreamtestingapp
    dreamdesktopduplicationapp
    dreamformapp
    dreamsettingsapp
    dreamloginapp
)

# External
target_link_libraries(testclient
    debug ${CMAKE_SOURCE_DIR}/src/third_party/CEF/CEF_BINARY_3538/Debug/libcef_dll_wrapper.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/CEF/CEF_BINARY_3538/Debug/libcef.dll.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/libcurl/curl-7.49.1/lib/Windows/x64/Debug/libcurl_imp.lib
    # These already linked by webrtc
    # debug ${CMAKE_SOURCE_DIR}/src/third_party/openssl/openssl-1.0.2/x64/Debug/libssl.lib
    # debug ${CMAKE_SOURCE_DIR}/src/third_party/openssl/openssl-1.0.2/x64/Debug/libcrypto.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/obj/webrtc.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/obj/webrtc_common.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/obj/api/audio_codecs/builtin_audio_decoder_factory.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/obj/api/audio_codecs/builtin_audio_encoder_factory.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/obj/api/audio_codecs/g711/audio_decoder_g711.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/obj/api/audio_codecs/g722/audio_decoder_g722.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/obj/api/audio_codecs/ilbc/audio_decoder_ilbc.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/obj/api/audio_codecs/isac/audio_decoder_isac_fix.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/obj/api/audio_codecs/isac/audio_decoder_isac_float.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/obj/api/audio_codecs/L16/audio_decoder_L16.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/obj/api/audio_codecs/opus/audio_decoder_opus.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/obj/media/rtc_audio_video.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/obj/modules/audio_processing/audio_processing.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/obj/pc/peerconnection.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/obj/rtc_base/rtc_base.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/obj/p2p/rtc_p2p.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/obj/system_wrappers/system_wrappers.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/obj/third_party/boringssl/boringssl.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/obj/third_party/boringssl/boringssl_asm.lib
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/jsoncpp/json_reader.obj
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/jsoncpp/json_writer.obj
    debug ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Debug/jsoncpp/json_value.obj
    #DreamOS.lib

    optimized ${CMAKE_SOURCE_DIR}/src/third_party/CEF/CEF_BINARY_3538/OfficialBuild/libcef_dll_wrapper.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/CEF/CEF_BINARY_3538/OfficialBuild/libcef.dll.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/libcurl/curl-7.49.1/lib/Windows/x64/Release/libcurl_imp.lib
    
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/obj/webrtc.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/obj/webrtc_common.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/obj/api/audio_codecs/builtin_audio_decoder_factory.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/obj/api/audio_codecs/builtin_audio_encoder_factory.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/obj/api/audio_codecs/g711/audio_decoder_g711.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/obj/api/audio_codecs/g722/audio_decoder_g722.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/obj/api/audio_codecs/ilbc/audio_decoder_ilbc.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/obj/api/audio_codecs/isac/audio_decoder_isac_fix.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/obj/api/audio_codecs/isac/audio_decoder_isac_float.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/obj/api/audio_codecs/L16/audio_decoder_L16.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/obj/api/audio_codecs/opus/audio_decoder_opus.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/obj/media/rtc_audio_video.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/obj/modules/audio_processing/audio_processing.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/obj/pc/peerconnection.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/obj/rtc_base/rtc_base.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/obj/p2p/rtc_p2p.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/obj/system_wrappers/system_wrappers.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/obj/third_party/boringssl/boringssl.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/obj/third_party/boringssl/boringssl_asm.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/jsoncpp/json_reader.obj
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/jsoncpp/json_writer.obj
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/webrtc/lib/x64/Release/jsoncpp/json_value.obj

    # HMD Binaries
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/OCULUS/v1.12.0/LibOVR/Lib/Windows/x64/Release/VS2015/LibOVR.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/OCULUS/OVRPlatformSDK_v1.24.0/Windows/LibOVRPlatform64_1.lib

    optimized ${CMAKE_SOURCE_DIR}/src/third_party/openvr/lib/win64/openvr_api.lib

    # These already linked by webrtc ?
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/openssl/openssl-1.0.2/x64/Release/libssl.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/openssl/openssl-1.0.2/x64/Release/libcrypto.lib

    # Freetype, freeimg, assimp
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/Freetype/freetype-2.8/objs/vc2010/x64/freetype28MT.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/assimp/3.3.1/bin/Release/assimp-vc140-mt.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/Freeimage/bin/x64/release/FreeImage.lib
    optimized ${CMAKE_SOURCE_DIR}/src/third_party/Freeimage/bin/x64/release/FreeImagePlus.lib

    #DreamOS.lib

    # Leap
    # TODO: Add a config flag to enable / disable
    ${CMAKE_SOURCE_DIR}/src/third_party/LeapSDK/lib/x64/Leap.lib

    

    # TODO: Make this into a windows dependent list 
    Xinput.lib
    dxgi.lib
    d3d11.lib
    XAudio2.lib
    HrtfApo.lib

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

# Post Build Copy Stuff Over
#add_custom_command(TARGET testclient POST_BUILD 
#  COMMAND "${CMAKE_COMMAND}" -E copy 
#     "$<TARGET_FILE:testclient>"
#     "my_target_path/$<CONFIGURATION>/$<TARGET_FILE_NAME:testclient>" 
#  COMMENT "Copying to output directory"
#  )

#add_custom_command(TARGET MyTest POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#        "${PROJECT_SOURCE_DIR}/libs"
#        $<TARGET_FILE_DIR:MyTest>)
#
#add_custom_command(TARGET MyTest POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#        "${PROJECT_SOURCE_DIR}/libs/$<CONFIGURATION>"
#        $<TARGET_FILE_DIR:MyTest>)

# TODO: Configure this for either configuration
file(GLOB CEF_BINARY_FILES              "${CMAKE_SOURCE_DIR}/src/third_party/CEF/CEF_BINARY_3538/OfficialBuild/*")
file(GLOB CURL_BINARY_FILES             "${CMAKE_SOURCE_DIR}/src/third_party/libcurl/curl-7.49.1/lib/Windows/x64/Release/*.dll")
file(GLOB LEAP_BINARY_FILES             "${CMAKE_SOURCE_DIR}/src/third_party/leapSDK/lib/x64/Leap.dll")
file(GLOB OPENVR_BINARY_FILES           "${CMAKE_SOURCE_DIR}/src/third_party/openvr/bin/win64/openvr_api.dll")
file(GLOB OCULUS_BINARY_FILES  
    "${CMAKE_SOURCE_DIR}/src/third_party/OCULUS/runtime/LibOVRPlatform64_1.dll"
    "${CMAKE_SOURCE_DIR}/src/third_party/OCULUS/runtime/LibOVRP2P64_1.dll"
)

file(GLOB OPENSSL_BINARY_FILES  
    "${CMAKE_SOURCE_DIR}/src/third_party/openssl/openssl-1.0.2/x64/Release/libssl-1_1-x64.dll"
    "${CMAKE_SOURCE_DIR}/src/third_party/openssl/openssl-1.0.2/x64/Release/libcrypto-1_1-x64.dll"
)
file(GLOB ASSIMP_BINARY_FILES  "${CMAKE_SOURCE_DIR}/src/third_party/assimp/3.3.1/bin/Release/assimp-vc140-mt.dll")
file(GLOB FREEIMAGE_BINARY_FILES  "${CMAKE_SOURCE_DIR}/src/third_party/Freeimage/bin/x64/release/FreeImage.dll")


add_custom_command(TARGET testclient POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CEF_BINARY_FILES}
    ${CURL_BINARY_FILES}
    ${LEAP_BINARY_FILES}
    ${OPENVR_BINARY_FILES}
    ${OCULUS_BINARY_FILES}
    ${OPENSSL_BINARY_FILES}
    ${ASSIMP_BINARY_FILES}
    ${FREEIMAGE_BINARY_FILES}
    $<TARGET_FILE_DIR:testclient>
)
