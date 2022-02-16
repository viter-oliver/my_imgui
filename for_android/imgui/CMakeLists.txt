link_libraries(json_lib)
include_directories($ENV{PKG_CONFIG_SYSROOT_DIR}/usr/include/freetype2)
include_directories(af_include)
include_directories(${afg_anroid_SOURCE_DIR}/json_lib)
#include_directories(${afg_anroid_SOURCE_DIR}/freetype)

include_directories(af_control fab  msgpack-c soil)

add_definitions(-DDISABLE_DEMO -DINCLUDE_ES3)

set(IMGUI imgui.cpp imgui_draw.cpp)
file(GLOB AF_CONTROL af_control/*.cpp af_control/*.c)
file(GLOB SOIL soil/*.c)
add_library(imgui_lib ${IMGUI} ${AF_CONTROL} ${SOIL})


set_target_properties(imgui_lib PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(imgui_lib EGL GLESv2 m pthread dl freetype)

#add_library(imgui_lib ${IMGUI} ${AF_CONTROL} ${SOIL} )
#set_target_properties(imgui_lib PROPERTIES LINKER_LANGUAGE CXX)
#target_link_libraries(imgui_lib wayland-client wayland-server wayland-cursor wayland-egl EGL GLESv2 m pthread dl)
