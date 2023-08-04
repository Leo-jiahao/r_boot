cmake_minimum_required(VERSION 3.1)



#add user head files to the end of compiler's include path list
include_directories(
    .
)

set(src
    uart.c
    
)


add_library(uart ${src})

# target_link_libraries(uart m)