project(scheduler)

add_executable(
    ${PROJECT_NAME}
    tcp_client.cpp
    sqllite3wrapper.cpp
    scheduler.cpp
)

FILE(GLOB_RECURSE LibFiles "*.h")
add_custom_target(
    ${PROJECT_NAME}_headers
    SOURCES ${LibFiles}
)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} sqlite3)
if(UNIX)
    target_link_libraries(${PROJECT_NAME} pthread)
endif()
