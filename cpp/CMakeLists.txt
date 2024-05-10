cmake_minimum_required(VERSION 3.27)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
include("C:/Users/sansk/OneDrive/Pictures/jkrgui/CMakeConfig.cmake")
include("C:/Users/sansk/OneDrive/Pictures/jkrgui/CMakeCommon.cmake")
include("C:/Users/sansk/OneDrive/Pictures/jkrgui/out/build/${CMAKE_PRESET_NAME}/luaExport.cmake")
include("C:/Users/sansk/OneDrive/Pictures/jkrgui/out/build/${CMAKE_PRESET_NAME}/ksaivulkanExport.cmake")
include("C:/Users/sansk/OneDrive/Pictures/jkrgui/out/build/${CMAKE_PRESET_NAME}/jkrengineExport.cmake")


project(hftsim)
add_library(hftsim SHARED hftsim.cpp hftsim.hpp)
target_link_libraries(hftsim lua ksaivulkan jkrengine)
configure_target(hftsim)

add_custom_command(
    TARGET hftsim POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:hftsim>
            ${CMAKE_CURRENT_SOURCE_DIR}/../
)
