add_definitions(-DLINUX -DEGL_API_FB -D_GLFW_DFB -DWL_EGL_PLATFORM  -D_GLFW_USE_CONFIG_H)
set(common_HEADERS internal.h
                   "${GLFW_SOURCE_DIR}/include/GLFW/glfw3.h"
                   "${GLFW_SOURCE_DIR}/include/GLFW/glfw3native.h")
set(common_SOURCES context.c init.c input.c monitor.c vulkan.c window.c)
set(glfw_HEADERS ${common_HEADERS} linux_joystick.h choose_config.h
                    posix_time.h posix_thread.h xkb_unicode.h egl_context.h)
set(glfw_SOURCES ${common_SOURCES} linux_joystick.c posix_time.c posix_thread.c xkb_unicode.c
                     egl_context.c choose_config.c)

add_library(glfw ${glfw_SOURCES} ${glfw_HEADERS})
set_target_properties(glfw PROPERTIES
                      OUTPUT_NAME ${GLFW_LIB_NAME}
                      VERSION ${GLFW_VERSION}
                      SOVERSION ${GLFW_VERSION_MAJOR}
                      POSITION_INDEPENDENT_CODE ON)
set(LIBRARY_OUTPUT_PATH ${GLFW_SOURCE_DIR}/afg_sdk/lib)