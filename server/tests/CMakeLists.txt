project(berkeley_server_tests)

file(GLOB PROJECT_SOURCES
    "*.h"
    "*.cpp"
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

find_package (Threads)
target_link_libraries(${PROJECT_NAME} halifax server_core gtest ${CMAKE_THREAD_LIBS_INIT})
