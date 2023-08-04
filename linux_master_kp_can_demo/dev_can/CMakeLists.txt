cmake_minimum_required(VERSION 3.1)



#add user head files to the end of compiler's include path list
include_directories(
    .
    ../kp_can
)

set(src
    dev_can.c
    
)


add_library(dev_can ${src})

target_link_libraries(dev_can kp_can)
