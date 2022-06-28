file(GLOB JSONCPP json/*.cpp)
add_library(json_lib ${JSONCPP})

set_target_properties(json_lib PROPERTIES LINKER_LANGUAGE CXX)
