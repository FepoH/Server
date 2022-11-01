cmake_minimum_required(VERSION 3.0)

project(fepoh_server)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS}  -g  -std=c++11 -Wno-deprecated -Wno-dev  -Wno-builtin-macro-redefined  -Wno-unused-function  -lstdc++")


include_directories(.)
include_directories(./tests)
include_directories(./src)
include_directories(./include)



find_library(JSON json)
message("*******************",${JSON})

find_package(Boost REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

set(LOG_SRC

    src/log/log_appender.cpp
    src/log/log_event.cpp
    src/log/log_formatter.cpp
    src/log/log_level.cpp
    src/log/log.cpp
    src/log/log_manager.cpp
    src/log/logger.cpp
    )

set(LIB_SRC 
    ${LOG_SRC}
    src/config.cpp
    src/fiber.cpp
    src/thread/thread.cpp
    src/thread/mutex.cpp
    src/thread/sem.cpp
    src/util.cpp
    src/schedule_manager.cpp
    src/timer.cpp
)



set(LIB_LIB
        yaml-cpp
        fepoh_server
        pthread
        
        
    )

add_library(fepoh_server SHARED ${LIB_SRC})





add_executable(test_log tests/test_log.cpp)
add_dependencies(test_log fepoh_server )
target_link_libraries(test_log ${LIB_LIB} )

add_executable(test_config tests/test_config.cpp)
add_dependencies(test_config fepoh_server )
target_link_libraries(test_config ${LIB_LIB} )

add_executable(test_json tests/test_json.cpp)
add_dependencies(test_json fepoh_server)
target_link_libraries(test_json fepoh_server ${LIB_LIB})


add_executable(test_thread tests/test_thread.cpp)
add_dependencies(test_thread fepoh_server)
target_link_libraries(test_thread fepoh_server ${LIB_LIB} )

add_executable(test_fiber tests/test_fiber.cpp)
add_dependencies(test_fiber fepoh_server)
target_link_libraries(test_fiber ${LIB_LIB})

add_executable(test_scheduler tests/test_scheduler.cpp)
add_dependencies(test_scheduler fepoh_server)
target_link_libraries(test_scheduler ${LIB_LIB})

add_executable(test_timer tests/test_timer.cpp)
add_dependencies(test_timer fepoh_server)
target_link_libraries(test_timer ${LIB_LIB})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
