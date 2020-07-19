# src/testclient/CMakeLists.txt - Dream Test Client Libraries CMake file

file(GLOB EDITOR_SOURCES 
  "*.cpp"
  "*.h"
)
source_group("" FILES ${EDITOR_SOURCES})

file(GLOB SOURCES 
  "DreamTestClient.cpp"
  "DreamTestClient.h"
)

file(GLOB WIN64_SOURCES 
  "DreamTestClientMain.cpp"
  "DreamTestClientMain.h"
)

file(GLOB ANDROID_SOURCES 
  "TestClientRenderer.cpp"
  "TestClientRenderer.h"
  "TestClientNativeActivity.cpp"
)


# TODO: This should not be here!!!!!
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/webrtc/include/webrtc)

# For OpenVR Matrix
include_directories(${CMAKE_SOURCE_DIR}/src/third_party/openvr/headers)

# Force Include the src/core/config.h
if(MSVC)
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/src/core/config.h")
else()
   # GCC or Clang
   add_definitions(-include "${CMAKE_SOURCE_DIR}/src/core/config.h")
endif()

# Both Configurations
set(DOS_DIR_VAL "${CMAKE_SOURCE_DIR}")
add_definitions(-DDOSROOTDIR="${DOS_DIR_VAL}")

# Release Configuration
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_CRT_SECURE_NO_WARNINGS")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNOMINMAX")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DCEF_AUDIO_MIRROR")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DSTAGING_BUILD")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_ENABLE_LOGGING")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DDOS_BASE_FILE__=\"%(Filename)%(Extension)\"")

IF(WIN32)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_WINDOWS")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DWEBRTC_WIN")
ELSEIF(ANDROID)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELEASE} -DWEBRTC_POSIX")
ENDIF()

# Debug Configuration
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_CONSOLE")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D__STD_C")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_CRT_SECURE_NO_DEPRECATE")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_SCL_SECURE_NO_DEPRECATE")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D__STDC_CONSTANT_MACROS")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D__STDC_FORMAT_MACROS")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_CRT_SECURE_NO_WARNINGS")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DNOMINMAX")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DENABLE_WEBRTC=1")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DENABLE_MEDIA_ROUTER=1")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DENABLE_PEPPER_CDMS")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DENABLE_SRTP=0")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_USE_MATH_DEFINES")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DCEF_AUDIO_MIRROR")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_ENABLE_LOGGING")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDOS_BASE_FILE=\"%(Filename)%(Extension)\"")

IF(WIN32)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DWEBRTC_WIN")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_WIN32_WINNT=0x0A00")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DWINVER=0x0A00")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DWIN32")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DWIN32_LEAN_AND_MEAN")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_WINSOCK_DEPRECATED_NO_WARNINGS")
ELSEIF(ANDROID)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DWEBRTC_POSIX")
ENDIF()


if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message("dos/src/testclient/cmakelists.txt msvc")

    # Compiler Flags Release
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi /W3 /WX /sdl /MP")      # General
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox /Ob2 /Oi /Ot /Oy")  # Optimization
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHsc /MT /Gy")         # Code Generation
    #set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /permissive-")          # Language
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /FC")                   # Advanced

    # Compiler Flags Debug
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /JMC /W3 /WX /sdl /MP")      # General
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od")  # Optimization
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Gm- /EHsc /RTC1 /MTd /GS /fp:precise")         # Code Generation
    #set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /permissive-")          # Language
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /FC")                   # Advanced

    # Linker flags (Release)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL")            # General
    # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB")           # Input
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST")               # Manifest File
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG:FASTLINK")         # Debugging
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")      # System

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")

    message("dos/src/testclient/cmakelists.txt apple clang")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOCONCEPTS")

	#SET(CMAKE_CXX_FLAGS -pthread)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    
    # Override certain warnings cause they're dumb
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Werror -Wfatal-errors -Wmacro-redefined -Wno-unused-variable -Wno-reorder -Wno-unused-parameter")

	# Debug Configuration
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++17 -fpermissive -stdlib=libc++ -fdeclspec")

	# Release Configuration
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++17 -fpermissive -stdlib=libc++ -fdeclspec")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

    message("dos/src/testclient/cmakelists.txt clang")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOCONCEPTS")

	#SET(CMAKE_CXX_FLAGS -pthread)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

    # Override certain warnings cause they're dumb
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Werror -Wfatal-errors -Wmacro-redefined -Wno-unused-variable -Wno-reorder -Wno-unused-parameter")

	# Debug Configuration
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++17 -fpermissive -stdlib=libc++ -fdeclspec")

	# Release Configuration
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++17 -fpermissive -stdlib=libc++ -fdeclspec")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	
	message("dos/src/testclient/cmakelists.txt GNU")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOCONCEPTS")

	#SET(CMAKE_CXX_FLAGS -pthread)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    
    # Override certain warnings cause they're dumb
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Werror -Wfatal-errors -Wmacro-redefined -Wno-unused-variable -Wno-reorder -Wno-unused-parameter")

	# Debug Configuration
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++17 -fpermissive -stdlib=libc++ -fdeclspec")

	# Release Configuration
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++17 -fpermissive -stdlib=libc++ -fdeclspec")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")


else()

	message("dos/src/testclient/cmakelists.txt other: ${CMAKE_CXX_COMPILER_ID}")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOCONCEPTS")

	#SET(CMAKE_CXX_FLAGS -pthread)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    
    # Override certain warnings cause they're dumb
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Werror -Wfatal-errors -Wmacro-redefined -Wno-unused-variable -Wno-reorder -Wno-unused-parameter")

	# Debug Configuration
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++17 -fpermissive -stdlib=libc++ -fdeclspec")

	# Release Configuration
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++17 -fpermissive -stdlib=libc++ -fdeclspec")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

endif()

IF(ANDROID)
    # # build the ndk-helper library
    # get_filename_component(ndkHelperDir ${CMAKE_SOURCE_DIR}/src/third_party/ndk-helpers ABSOLUTE)
    # get_filename_component(ndkHelperSrc ${ndkHelperDir}/ndk_helper ABSOLUTE)
    # #add_subdirectory(${ndkHelperSrc}
    # #        ${ndkHelperDir}/ndkHelperBin/${CMAKE_BUILD_TYPE}/${ANDROID_ABI})

    # add_subdirectory(${ndkHelperSrc})

    # Add the NDK helpers dir (currently in testclient, should be moved to sandbox/android)
    add_subdirectory(ndk-helpers/ndk_helper ndk-helpers/ndkHelperBin/${CMAKE_BUILD_TYPE}/${ANDROID_ABI} EXCLUDE_FROM_ALL)

    # The Android Build
    add_library(TestClientNativeActivity
        SHARED
            #${SOURCES}
            ${ANDROID_SOURCES}
    )

    set_target_properties(TestClientNativeActivity
        PROPERTIES
            CXX_STANDARD 11
            CXX_STANDARD_REQUIRED YES
            CXX_EXTENSIONS NO
    )

    #target_link_libraries(testclient core)
    #target_link_libraries(testclient os)
    #target_link_libraries(testclient sandbox)
    #target_link_libraries(testclient test)
    #target_link_libraries(testclient logger)
    #target_link_libraries(testclient modules)

    # Export ANativeActivity_onCreate(),
    # Refer to: https://github.com/android-ndk/ndk/issues/381.
    set_target_properties(TestClientNativeActivity
        PROPERTIES  LINK_FLAGS "-u ANativeActivity_onCreate")

    # add ndk helped lib dependencies
    target_link_libraries(TestClientNativeActivity
        PRIVATE
            NdkHelper
            core
            #sandbox
    )

ELSEIF(WIN32)
    # The Windows Build
    add_executable(testclient
        ${SOURCES}
        ${WIN64_SOURCES}
    )

    set_target_properties(testclient PROPERTIES 
        #VS_DEBUGGER_COMMAND "debug_command"
        VS_DEBUGGER_COMMAND_ARGUMENTS "-ts sandbox.win64 -t 0"
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
        # Optimized Configuration
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
        #DreamOS.lib

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

        # optimized msvcrt.lib 
        # optimized vcruntime.lib
        # optimized ucrt.lib
        optimized libcmt.lib 

        # Debug Configuration
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

        # HMD Binaries
        debug ${CMAKE_SOURCE_DIR}/src/third_party/OCULUS/v1.12.0/LibOVR/Lib/Windows/x64/Debug/VS2015/LibOVR.lib
        debug ${CMAKE_SOURCE_DIR}/src/third_party/OCULUS/OVRPlatformSDK_v1.24.0/Windows/LibOVRPlatform64_1.lib

        debug ${CMAKE_SOURCE_DIR}/src/third_party/openvr/lib/win64/openvr_api.lib

        # These already linked by webrtc ?
        #debug ${CMAKE_SOURCE_DIR}/src/third_party/openssl/openssl-1.0.2/x64/Debug/libssl.lib
        #debug ${CMAKE_SOURCE_DIR}/src/third_party/openssl/openssl-1.0.2/x64/Debug/libcrypto.lib
        debug ${CMAKE_SOURCE_DIR}/src/third_party/openssl/openssl-1.0.2/x64/Release/libssl.lib
        debug ${CMAKE_SOURCE_DIR}/src/third_party/openssl/openssl-1.0.2/x64/Release/libcrypto.lib

        # Freetype, freeimg, assimp
        debug ${CMAKE_SOURCE_DIR}/src/third_party/Freetype/freetype-2.8/objs/vc2010/x64/freetype28MT.lib
        debug ${CMAKE_SOURCE_DIR}/src/third_party/assimp/3.3.1/bin/Debug/assimp-vc140-mt.lib
        debug ${CMAKE_SOURCE_DIR}/src/third_party/Freeimage/bin/x64/debug/FreeImaged.lib
        debug ${CMAKE_SOURCE_DIR}/src/third_party/Freeimage/bin/x64/debug/FreeImagePlusd.lib
        #DreamOS.lib

        debug msvcrtd.lib
        debug vcruntimed.lib
        # debug ucrtd.lib
        debug libcmtd.lib 
        # Common (optimized and debug)

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
        msdmo.lib
        crypt32.lib
        iphlpapi.lib
        secur32.lib
        Strmiids.lib
        amstrmid.lib
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

    file(GLOB FREEIMAGE_BINARY_FILES  
        "${CMAKE_SOURCE_DIR}/src/third_party/Freeimage/bin/x64/release/FreeImage.dll"
        "${CMAKE_SOURCE_DIR}/src/third_party/Freeimage/bin/x64/debug/FreeImaged.dll"
        "${CMAKE_SOURCE_DIR}/src/third_party/Freeimage/bin/x64/release/FreeImagePlus.dll"
        "${CMAKE_SOURCE_DIR}/src/third_party/Freeimage/bin/x64/debug/FreeImagePlusd.dll"
    )

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

    # Copy CEF files
    add_custom_command(TARGET testclient POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy 
        $<TARGET_FILE:dreamcef> 
        $<TARGET_FILE_DIR:testclient>
    )

    add_custom_command(TARGET testclient POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
        "${CMAKE_SOURCE_DIR}/src/third_party/CEF/CEF_BINARY_3538/Resources"
        $<TARGET_FILE_DIR:testclient>
    )

    # Copy Desktop Duplication files
    add_custom_command(TARGET testclient POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy 
        $<TARGET_FILE:dreamdesktop> 
        $<TARGET_FILE_DIR:dreammeetclient>
    )
ENDIF()