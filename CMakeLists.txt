cmake_minimum_required(VERSION 3.0)

project(fepoh_server)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS}  -g  -std=c++11 -Wno-deprecated -Wno-dev  -Wno-builtin-macro-redefined  -Wno-unused-function  -lstdc++")


include_directories(.)
include_directories(./tests)
include_directories(./src)
include_directories(./include)
include_directories(./resource)
include_directories(./examples)
include(./util.cmake)



#find_library(JSON json)
#message("*******************",${JSON})

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
    src/io_manager.cpp
    src/hook.cpp
    src/fd_manager.cpp
    src/address.cpp
    src/socket_.cpp
    src/bytearray.cpp
    src/http/http.cpp
    src/http/http_parser.cpp
    src/http/http_parser/http_nodejs_parser.c
    src/tcp_server.cpp
    src/stream.cpp
    src/socket_stream.cpp
    src/http/http_session.cpp
    src/http/http_server.cpp
    src/http/servlet.cpp
    src/http/http_connection.cpp
    src/uri.cpp
)


set(LIB_LIB
    yaml-cpp
    fepoh_server
    pthread
    dl
)

add_library(fepoh_server SHARED ${LIB_SRC})
redefine_file_macro(fepoh_server)


function(fepoh_add_executable exe_name "src_file" dependencies link_lib)
    add_executable(${exe_name} ${src_file})
    redefine_file_macro(${exe_name})
    add_dependencies(${exe_name} ${dependencies})
    target_link_libraries(${exe_name} ${link_lib})
endfunction()

fepoh_add_executable(test_log "tests/test_log.cpp" fepoh_server "${LIB_LIB}")
fepoh_add_executable(test_config "tests/test_config.cpp" fepoh_server "${LIB_LIB}")
fepoh_add_executable(test_json "tests/test_json.cpp" fepoh_server "${LIB_LIB}")
fepoh_add_executable(test_thread "tests/test_thread.cpp" fepoh_server "${LIB_LIB}")
# test_fiber.cpp失效,通过IOManager测试Fiber
# fepoh_add_executable(test_fiber "tests/test_fiber.cpp" fepoh_server "${LIB_LIB}")

fepoh_add_executable(test_scheduler "tests/test_scheduler.cpp" fepoh_server "${LIB_LIB}")
fepoh_add_executable(test_timer "tests/test_timer.cpp" fepoh_server "${LIB_LIB}")
fepoh_add_executable(test_iomanager "tests/test_iomanager.cpp" fepoh_server "${LIB_LIB}")
fepoh_add_executable(test_hook "tests/test_hook.cpp" fepoh_server "${LIB_LIB}")
fepoh_add_executable(test_address "tests/test_address.cpp" fepoh_server "${LIB_LIB}")
fepoh_add_executable(test_socket "tests/test_socket.cpp" fepoh_server "${LIB_LIB}")
fepoh_add_executable(test_client "tests/test_client.cpp" fepoh_server "${LIB_LIB}")
fepoh_add_executable(test_sockstream "tests/test_sockstream.cpp" fepoh_server "${LIB_LIB}")

fepoh_add_executable(test_bytearray "tests/test_bytearray.cpp" fepoh_server "${LIB_LIB}")
fepoh_add_executable(test_http "tests/test_http.cpp" fepoh_server "${LIB_LIB}")
fepoh_add_executable(test_http_parser "tests/test_http_parser.cpp" fepoh_server "${LIB_LIB}")

fepoh_add_executable(test_tcp_server "tests/test_tcp_server.cpp" fepoh_server "${LIB_LIB}")
fepoh_add_executable(test_http_server "tests/test_http_server.cpp" fepoh_server "${LIB_LIB}")

fepoh_add_executable(test_uri "tests/test_uri.cpp" fepoh_server "${LIB_LIB}")
fepoh_add_executable(test_connection "tests/test_connection.cpp" fepoh_server "${LIB_LIB}")

# #examples
fepoh_add_executable(echo_server "examples/echo_server.cpp" fepoh_server "${LIB_LIB}")
fepoh_add_executable(my_http "examples/my_http.cpp" fepoh_server "${LIB_LIB}")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)










# add_executable(test_log tests/test_log.cpp)
# add_dependencies(test_log fepoh_server)
# target_link_libraries(test_log ${LIB_LIB} )

# add_executable(test_config tests/test_config.cpp)
# add_dependencies(test_config fepoh_server )
# target_link_libraries(test_config ${LIB_LIB} )

# add_executable(test_json tests/test_json.cpp)
# add_dependencies(test_json fepoh_server)
# target_link_libraries(test_json fepoh_server ${LIB_LIB})

# add_executable(test_thread tests/test_thread.cpp)
# add_dependencies(test_thread fepoh_server)
# target_link_libraries(test_thread fepoh_server ${LIB_LIB} )

# add_executable(test_fiber tests/test_fiber.cpp)
# add_dependencies(test_fiber fepoh_server)
# target_link_libraries(test_fiber ${LIB_LIB})

# add_executable(test_scheduler tests/test_scheduler.cpp)
# add_dependencies(test_scheduler fepoh_server)
# target_link_libraries(test_scheduler ${LIB_LIB})

# add_executable(test_timer tests/test_timer.cpp)
# add_dependencies(test_timer fepoh_server)
# target_link_libraries(test_timer ${LIB_LIB})

# add_executable(test_iomanager tests/test_iomanager.cpp)
# add_dependencies(test_iomanager fepoh_server)
# target_link_libraries(test_iomanager ${LIB_LIB})

# add_executable(test_hook tests/test_hook.cpp)
# add_dependencies(test_hook fepoh_server)
# target_link_libraries(test_hook ${LIB_LIB})

# add_executable(test_address tests/test_address.cpp)
# add_dependencies(test_address fepoh_server)
# target_link_libraries(test_address ${LIB_LIB})