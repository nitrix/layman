PROJECT(toolkit)

ADD_LIBRARY(
    toolkit
    src/buffer.c
    src/file.c
    src/time.c
)

TARGET_INCLUDE_DIRECTORIES(toolkit PUBLIC include)