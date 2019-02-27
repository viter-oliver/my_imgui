link_libraries(glfw)
include_directories(${GLFW_SOURCE_DIR}/include)
include_directories("${GLFW_SOURCE_DIR}/deps")
add_definitions(-DGLFW_INCLUDE_ES3)
#set(GLAD "${GLFW_SOURCE_DIR}/deps/glad/glad.h" "${GLFW_SOURCE_DIR}/deps/glad.c")
#set(GETOPT "${GLFW_SOURCE_DIR}/deps/getopt.h" "${GLFW_SOURCE_DIR}/deps/getopt.c")
set(TINYCTHREAD "${GLFW_SOURCE_DIR}/deps/tinycthread.h"
                "${GLFW_SOURCE_DIR}/deps/tinycthread.c")
add_executable(single_window single_window.c)
target_link_libraries(single_window wayland-client wayland-server wayland-cursor
 wayland-egl EGL GLESv2 m pthread dl)
