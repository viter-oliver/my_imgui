
include_directories(${GLFW_SOURCE_DIR}/boost_1_65_1)
include_directories(${GLFW_SOURCE_DIR}/afg_sdk/include)
include_directories(${GLFW_SOURCE_DIR}/afg_sdk/include/message)
include_directories(${GLFW_SOURCE_DIR}/afg_sdk/include/imgui)
include_directories(${GLFW_SOURCE_DIR}/afg_sdk/include/imgui/af_include)
include_directories(${GLFW_SOURCE_DIR}/middle_ware/include)
include_directories(${GLFW_SOURCE_DIR}/middle_ware/include/IPC)
include_directories($ENV{PKG_CONFIG_SYSROOT_DIR}/usr/include/freetype2)

link_directories(${GLFW_SOURCE_DIR}/afg_sdk/lib)
link_directories(${GLFW_SOURCE_DIR}/middle_ware/lib)
file(GLOB GP_CPP ./*.cpp *.c)
add_definitions(-DGLFW_INCLUDE_ES3)
add_definitions(-DIMGUI_DISABLE_DEMO_WINDOWS)
add_definitions(-D_GLFW_DFB)
add_executable(RV_app ${GP_CPP})
set_target_properties(RV_app PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(RV_app imgui_lib json_lib glfw message_lib  EGL GLESv2 m pthread python3.5m dl freetype)
#target_link_libraries(hq_app imgui_lib json_lib glfw message_lib wayland-client wayland-server wayland-cursor EGL GLESv2 m pthread python2.7 dl freetype)


