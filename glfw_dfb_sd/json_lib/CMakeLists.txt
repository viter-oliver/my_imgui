file(GLOB JSONCPP json/*.cpp)
add_library(json_lib ${JSONCPP})
set(LIBRARY_OUTPUT_PATH ${GLFW_SOURCE_DIR}/afg_sdk/lib)
set_target_properties(json_lib PROPERTIES LINKER_LANGUAGE CXX)
