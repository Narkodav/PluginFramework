cmake_minimum_required(VERSION 3.20)
project(PluginFramework LANGUAGES CXX)

# ===============================
# Library target
# ===============================

file(GLOB_RECURSE SRC_FILES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(PluginFramework ${SRC_FILES})
add_library(PluginFramework::PluginFramework ALIAS PluginFramework)

target_compile_features(PluginFramework PUBLIC cxx_std_20)

target_include_directories(PluginFramework
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_options(PluginFramework PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE LIBRARY_LOADER_EXPORTS
)

# =========================
# Test plugin (shared lib)
# =========================

add_library(TestPlugin SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/mod.cpp
)

# Ensure no "lib" prefix on Unix if you want exact name
set_target_properties(TestPlugin PROPERTIES
    PREFIX ""
    OUTPUT_NAME "test_plugin"
)

# Make sure symbols are exported (especially on Windows)
target_compile_definitions(TestPlugin PRIVATE TEST_PLUGIN_EXPORTS)

target_include_directories(TestPlugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

target_compile_features(TestPlugin PUBLIC cxx_std_20)
target_compile_options(TestPlugin PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# =========================
# Test executable
# =========================
file(GLOB_RECURSE TEST_SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
)

add_executable(PluginFrameworkTests ${TEST_SRC_FILES})

# Link the library
target_link_libraries(PluginFrameworkTests PRIVATE ${PROJECT_NAME})

# Match C++ standard and warnings
target_compile_features(PluginFrameworkTests PUBLIC cxx_std_20)
target_compile_options(PluginFrameworkTests PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

target_include_directories(PluginFrameworkTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Enable CTest
enable_testing()
add_test(NAME PluginFrameworkTests COMMAND PluginFrameworkTests)

# ===============================
# Install & package (TOP-LEVEL ONLY)
# ===============================

install(TARGETS ${PROJECT_NAME} #LibraryLoader_stb_image
    EXPORT LibraryLoaderTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/
    DESTINATION include
)

install(EXPORT LibraryLoaderTargets
    FILE LibraryLoaderTargets.cmake
    NAMESPACE PluginFramework::
    DESTINATION lib/cmake/PluginFramework
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/install/PluginFrameworkConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/PluginFrameworkConfig.cmake
    INSTALL_DESTINATION lib/cmake/PluginFramework
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/PluginFrameworkConfig.cmake
    DESTINATION lib/cmake/PluginFramework
)

add_custom_command(TARGET PluginFrameworkTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:TestPlugin>
        $<TARGET_FILE_DIR:PluginFrameworkTests>
)