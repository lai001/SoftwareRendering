include(ExternalProject)

CMAKE_MINIMUM_REQUIRED( VERSION 3.16 )
PROJECT(SoftwareRendering VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
include_directories("./")
aux_source_directory("./" SRC)
file(GLOB HEADERS *.hpp)

add_subdirectory(Vendor/assimp)
add_subdirectory(Vendor/spdlog)

add_executable(${PROJECT_NAME} ${SRC} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE assimp)
target_include_directories(${PROJECT_NAME} PRIVATE Vendor/assimp/include)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)
target_include_directories(${PROJECT_NAME} PRIVATE Vendor/spdlog/include)

set(ASSIMP_LIBRARY_SUFFIX -vc141-mt)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${Assimp_BINARY_DIR}/code/$<CONFIG>/assimp${ASSIMP_LIBRARY_SUFFIX}$<$<CONFIG:Debug>:d>.dll ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${spdlog_BINARY_DIR}/$<CONFIG>/spdlog$<$<CONFIG:Debug>:d>.dll ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/Resource ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/Resource)

message(STATUS "--------------------------------------------------")
get_cmake_property(_variableNames VARIABLES)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()