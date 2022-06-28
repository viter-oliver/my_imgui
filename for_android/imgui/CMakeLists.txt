link_libraries(json_lib)
include_directories(af_include)
include_directories(${RendererJNI_SOURCE_DIR}/boost_1_65_1)
include_directories(${RendererJNI_SOURCE_DIR}/json_lib)
include_directories(${RendererJNI_SOURCE_DIR}/freetype/include)
include_directories(${RendererJNI_SOURCE_DIR}/imgui)
include_directories(${RendererJNI_SOURCE_DIR}/lua)
include_directories(af_control fab  msgpack-c soil)

add_definitions(-DDISABLE_DEMO -DINCLUDE_ES3 -DGL_GLEXT_PROTOTYPES)

set(IMGUI imgui.cpp imgui_draw.cpp)
file(GLOB AF_CONTROL af_control/*.cpp af_control/*.c)
file(GLOB SOIL soil/*.c)
file(GLOB LUA ${RendererJNI_SOURCE_DIR}/lua/*.c)
add_library(imgui_lib ${IMGUI} ${AF_CONTROL} ${SOIL} ${LUA})


set_target_properties(imgui_lib PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(imgui_lib EGL GLESv3 m dl freetype json_lib)

#add_library(imgui_lib ${IMGUI} ${AF_CONTROL} ${SOIL} )
#set_target_properties(imgui_lib PROPERTIES LINKER_LANGUAGE CXX)
#target_link_libraries(imgui_lib wayland-client wayland-server wayland-cursor wayland-egl EGL GLESv3 m pthread dl)
