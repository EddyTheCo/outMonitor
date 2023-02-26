cmake_minimum_required(VERSION 3.16)
project(IOTA VERSION 0.1 LANGUAGES CXX)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick)

qt6_add_qml_module(bout_monitor
    URI  bout_monitor
    VERSION 1.0
    SOURCES
    src/boutmonitor.cpp include/boutmonitor.hpp
    RESOURCE_PREFIX
   "/esterVtech.com/imports"
    OUTPUT_TARGETS out_targets_var
    NO_IMPORT_SCAN
    IMPORTS nodeConection
    IMPORT_PATH
    ${CMAKE_BINARY_DIR}/_deps/nodeconection-build
    OUTPUT_DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/bout_monitor
)

target_link_libraries(bout_monitor PUBLIC Qt6::Gui Qt6::Quick)

target_include_directories(bout_monitor PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        "$<INSTALL_INTERFACE:include>")

include(FetchContent)

FetchContent_Declare(
    nodeConection
    GIT_REPOSITORY git@github.com:EddyTheCo/ConectionSettings.git
    GIT_TAG main
)
FetchContent_MakeAvailable(nodeConection)

FetchContent_Declare(
        qbech32
        GIT_REPOSITORY git@github.com:EddyTheCo/Qbech32.git
        GIT_TAG main
        )
FetchContent_MakeAvailable(qbech32)


target_link_libraries(bout_monitor PUBLIC nodeConection)
target_link_libraries(bout_monitor PRIVATE qbech32)

target_compile_definitions(bout_monitor
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

install(TARGETS bout_monitor ${out_targets_var} EXPORT bout_monitorTargets DESTINATION bin )
install(DIRECTORY include/ DESTINATION include/)

install(EXPORT bout_monitorTargets
    FILE bout_monitorTargets.cmake
    DESTINATION lib/cmake/bout_monitor
)
include(CMakePackageConfigHelpers)
# generate the config file that is includes the exports
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/bout_monitorConfig.cmake"
    INSTALL_DESTINATION "lib/cmake/bout_monitor"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/bout_monitorConfigVersion.cmake"
    VERSION "0.1.1.0"
    COMPATIBILITY AnyNewerVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/bout_monitorConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/bout_monitorConfigVersion.cmake
    DESTINATION lib/cmake/bout_monitor
)
export(EXPORT bout_monitorTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/bout_monitorTargets.cmake"
)
list(APPEND QML_IMPORT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/qml ${CMAKE_CURRENT_BINARY_DIR}/)
set(QML_IMPORT_PATH ${QML_IMPORT_PATH}  CACHE STRING "" FORCE)

