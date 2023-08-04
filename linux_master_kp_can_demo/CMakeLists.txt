cmake_minimum_required(VERSION 3.1)

set(CMAKE_C_FLAGS "-std=gnu11 -g")

set(src
    main.c
)

set(CMAKE_C_COMPILER

    "/home/user/tools/esm6800/sysroots/x86_64-pokysdk-linux/usr/bin/arm-emtronix-linux-gnueabi/arm-emtronix-linux-gnueabi-gcc"
)

#set exe file path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(r_boot/master)
add_subdirectory(dev_can)
add_subdirectory(kp_can)

include_directories(
    r_boot/master
    dev_can/
    /home/user/tools/esm6800/sysroots/x86_64-pokysdk-linux/usr/include
    kp_can
)

add_executable(r_boot_kp_can_demo ${src})

target_link_libraries(r_boot_kp_can_demo 
    r_bootm 
    dev_can 
    pthread
    kp_can
)

