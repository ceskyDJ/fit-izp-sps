cmake_minimum_required(VERSION 3.17)
project(IZP\ |\ SPS\ \(2nd\ project\)\ -\ Development C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

add_executable(sps_dev main.c)