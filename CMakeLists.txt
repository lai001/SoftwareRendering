include(ExternalProject)

CMAKE_MINIMUM_REQUIRED( VERSION 3.16 )
PROJECT(SoftwareRendering VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

include_directories("./")
include_directories("Vendor")
include_directories("Vendor/glad/include")
aux_source_directory("./" SRC)
file(GLOB HEADERS *.hpp)

add_executable(${PROJECT_NAME} Vendor/glad/include/glad/glad.h Vendor/glad/src/glad.c ${SRC} ${HEADERS})

add_subdirectory(Vendor/assimp)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp)
target_include_directories(${PROJECT_NAME} PRIVATE Vendor/assimp/include)

add_subdirectory(Vendor/spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)
target_include_directories(${PROJECT_NAME} PRIVATE Vendor/spdlog/include)

add_subdirectory(Vendor/glm)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(Vendor/glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_include_directories(${PROJECT_NAME} PRIVATE Vendor/glfw/include)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

set(ASSIMP_LIBRARY_SUFFIX -vc141-mt)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${Assimp_BINARY_DIR}/code/$<CONFIG>/assimp${ASSIMP_LIBRARY_SUFFIX}$<$<CONFIG:Debug>:d>.dll ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${spdlog_BINARY_DIR}/$<CONFIG>/spdlog$<$<CONFIG:Debug>:d>.dll ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${GLFW_BINARY_DIR}/src/$<CONFIG>/glfw3$<$<CONFIG:Debug>:d>.dll ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/Resource ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/Resource)

message(STATUS "--------------------------------------------------")
get_cmake_property(_variableNames VARIABLES)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()