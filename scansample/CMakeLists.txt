project(scansample DESCRIPTION "scansample.")

add_executable(${PROJECT_NAME} scansample.cpp)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Scanner
)

#add_test(NAME ${PROJECT_NAME} COMMAND $<TARGET_FILE:${PROJECT_NAME}>)
