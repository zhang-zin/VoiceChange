cmake_minimum_required(VERSION 3.10.2)

project("voicechange")

include_directories("inc")

file(GLOB allCPP *.c *.h *.cpp)

add_library(
        native-lib
        SHARED
        ${allCPP})

find_library(
        log-lib
        log)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/../jniLibs/${CMAKE_ANDROID_ARCH_ABI}")

target_link_libraries(
        native-lib
        ${log-lib}
        fmod
        fmodl)