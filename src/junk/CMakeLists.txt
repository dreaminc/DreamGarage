# src/junk/CMakeList.txt : Junk projects such as init projects 
# to learn new APIs etc

# Force Include the src/core/config.h
if(MSVC)
    add_definitions(/FI"${CMAKE_SOURCE_DIR}/src/core/config.h")
#else()
#    # GCC or Clang
#    add_definitions(-include ${CMAKE_SOURCE_DIR}/src/core/config.h)
endif()

# Both Configurations
set(DOS_DIR_VAL "${CMAKE_SOURCE_DIR}")
add_definitions(-DDOSROOTDIR="${DOS_DIR_VAL}")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

    message("dos/src/junk/vulkaninitproj/cmakelists.txt msvc")

    # Release Configuration
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_WINDOWS")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_CRT_SECURE_NO_WARNINGS")
    # set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_ENABLE_LOGGING")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D__BASE_FILE__=\"%(Filename)%(Extension)\"")

    # Debug Configuration
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_CONSOLE")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D__STD_C")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_CRT_SECURE_NO_DEPRECATE")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_SCL_SECURE_NO_DEPRECATE")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D__STDC_CONSTANT_MACROS")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D__STDC_FORMAT_MACROS")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_WIN32_WINNT=0x0A00")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DWINVER=0x0A00")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DWIN32")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DWIN32_LEAN_AND_MEAN")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_USE_MATH_DEFINES")
    # set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_ENABLE_LOGGING")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D__BASE_FILE__=\"%(Filename)%(Extension)\"")

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

    message("dos/src/junk/vulkaninitproj/cmakelists.txt apple clang")

#    message("flags: ${CMAKE_CXX_FLAGS}")
#    message("debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
#    message("release flags: ${CMAKE_CXX_FLAGS_RELEASE}")

else()

    message("Warning: dos/src/meetclient/cmakelists.txt ${CMAKE_CXX_COMPILER_ID} not handled")

endif()

add_subdirectory(vulkaninitproj)