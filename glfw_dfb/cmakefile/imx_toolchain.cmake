SET(CMAKE_SYSTEM_NAME Linux)
# compiler
set(MYSYSROOT $ENV{PKG_CONFIG_SYSROOT_DIR})
SET(CMAKE_CXX_COMPILER $ENV{OECORE_NATIVE_SYSROOT}/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-g++)
#set(MYSYSROOT /opt/fsl-imx-wayland/4.1.15-2.0.0/sysroots/cortexa9hf-neon-poky-linux-gnueabi/ /opt/fsl-imx-wayland/4.1.15-2.0.0/sysroots/cortexa9hf-neon-poky-linux-gnueabi/usr/lib/ /opt/fsl-imx-wayland/4.1.15-2.0.0/sysroots/x86_64-pokysdk-linux/)
set(qt_wayland ${MYSYSROOT}/usr/lib/qt5/plugins/wayland-graphics-integration-client/)

# compiler/linker flags
#set(CMAKE_STRIP /opt/fsl-imx-wayland/4.1.15-2.0.0/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-strip)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a -mfpu=neon  -mfloat-abi=hard -mcpu=cortex-a9 --sysroot=${MYSYSROOT}" CACHE INTERNAL "" FORCE)
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} --sysroot=${MYSYSROOT}" CACHE INTERNAL "" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a -mfpu=neon  -mfloat-abi=hard -mcpu=cortex-a9 --sysroot=${MYSYSROOT}" CACHE INTERNAL "" FORCE)
set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} --sysroot=${MYSYSROOT}" CACHE INTERNAL "" FORCE)
# cmake built-in settings to use find_xxx() functions
set(CMAKE_FIND_ROOT_PATH "${MYSYSROOT} ${qt_wayland}")

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
