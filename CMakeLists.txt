CMAKE_MINIMUM_REQUIRED(VERSION 3.7)

SET(CMAKE_C_COMPILER gcc)
SET(CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -Werror -g")

PROJECT(ifj19_compiler)

FILE(GLOB SOURCES "*.c")
FILE(GLOB HEADERS "*.h")
FILE(GLOB TEXT "*.txt")

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${TEXT} main.c stack.c stack.h parser.c parser.h symtable.c symtable.h symtable.c symtable.h)