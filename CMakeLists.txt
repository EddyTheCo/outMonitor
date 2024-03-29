cmake_minimum_required(VERSION 3.24)

include(${CMAKE_CURRENT_BINARY_DIR}/local_conf.cmake OPTIONAL)

include(FetchContent)
FetchContent_Declare(
	ccommon
	GIT_REPOSITORY https://github.com/EddyTheCo/Common.git
	GIT_TAG main
	)
FetchContent_MakeAvailable(ccommon)
version_from_git(
	LOG       OFF
	TIMESTAMP "%Y%m%d%H%M%S"
	)
project(outMonitorQml VERSION ${VERSION} DESCRIPTION "Output Monitor QML module" LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
include(GNUInstallDirs)

FetchContent_Declare(
	nodeConection
	GIT_REPOSITORY https://github.com/EddyTheCo/ConectionSettings.git
	GIT_TAG v0.4.1
	FIND_PACKAGE_ARGS 0.4 CONFIG
	)
FetchContent_MakeAvailable(nodeConection)

FetchContent_Declare(
	qbech32
	GIT_REPOSITORY https://github.com/EddyTheCo/Qbech32.git
	GIT_TAG v0.0.2
	FIND_PACKAGE_ARGS 0.0 CONFIG
	)
FetchContent_MakeAvailable(qbech32)



find_package(Qt6 COMPONENTS Core Gui Quick)

if (Qt6_FOUND)
	qt_standard_project_setup()
	qt6_add_qml_module(outMonitor
		URI  outMonitor
		VERSION 1.0
		SOURCES
		src/outmonitor.cpp include/outmonitor.hpp
		OUTPUT_TARGETS out_targets_var
		RESOURCE_PREFIX
		"/esterVtech.com/imports"
		OUTPUT_DIRECTORY
		${CMAKE_BINARY_DIR}/outMonitor
		IMPORT_PATH ${CMAKE_BINARY_DIR}
		)

	target_compile_definitions(outMonitor PRIVATE WINDOWS_OMONI)
	target_link_libraries(outMonitor PUBLIC Qt6::Gui Qt6::Quick)

	target_link_libraries(outMonitor PUBLIC nodeConection
		$<$<STREQUAL:$<TARGET_PROPERTY:nodeConection,TYPE>,STATIC_LIBRARY>:nodeConectionplugin>
		)
	target_link_libraries(outMonitor PRIVATE qbech32)


	target_include_directories(outMonitor PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/outMonitor>")

	install(TARGETS outMonitor outMonitorplugin ${out_targets_var}
		EXPORT outMonitorTargets
		DESTINATION ${CMAKE_INSTALL_LIBDIR}
		)
	install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/outMonitor
		)

	install(DIRECTORY ${CMAKE_BINARY_DIR}/outMonitor/
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/QMLPlugins/outMonitor
		)

	install(EXPORT outMonitorTargets
		FILE outMonitorTargets.cmake
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/outMonitor
		)
	include(CMakePackageConfigHelpers)
	configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
		"${CMAKE_CURRENT_BINARY_DIR}/outMonitorConfig.cmake"
		INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/outMonitor
		)
	write_basic_package_version_file(
		"${CMAKE_CURRENT_BINARY_DIR}/outMonitorConfigVersion.cmake"
		VERSION ${VERSION}
		COMPATIBILITY SameMajorVersion
		)
	install(FILES
		${CMAKE_CURRENT_BINARY_DIR}/outMonitorConfig.cmake
		${CMAKE_CURRENT_BINARY_DIR}/outMonitorConfigVersion.cmake
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/outMonitor
		)
	export(EXPORT outMonitorTargets
		FILE "${CMAKE_CURRENT_BINARY_DIR}/outMonitorTargets.cmake"
		)
	if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
		set(CPACK_PACKAGE_CONTACT "estervtech")
		include(CTest)
		set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-v${SEMVER}-${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_VERSION}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_CXX_COMPILER_ID}")
		if(USE_THREADS)
			set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-MThread")
		endif(USE_THREADS)
		include(CPack)
	endif()
endif(Qt6_FOUND)

if(BUILD_DOCS)
	get_target_property(build_docs cmake_build_docs SOURCES)
	include(${build_docs})
endif()

