find_package(Qt5 COMPONENTS Core Qml REQUIRED)

set(CMAKE_AUTOMOC ON)

set(src_files
		qffuture.cpp
		)

# private header
set(header_files
		qfvariantwrapper.h
		qffuture.h
		QuickFuture
		quickfuture.h
		)

set(quickfutureqmlplugin_outputDir "${CMAKE_CURRENT_BINARY_DIR}/QuickFuture")
add_library(quickfutureqmlplugin SHARED ${src_files} ${header_files})
target_compile_definitions(quickfutureqmlplugin PUBLIC QUICKFUTURE_QML_PATH="${CMAKE_CURRENT_BINARY_DIR}" PRIVATE QUICK_FUTURE_BUILD_PLUGIN)
set_target_properties(quickfutureqmlplugin
											PROPERTIES
											OUTPUT_NAME "quickfutureqmlplugin"
											LIBRARY_OUTPUT_DIRECTORY "${quickfutureqmlplugin_outputDir}"
											LIBRARY_OUTPUT_DIRECTORY_RELEASE "${quickfutureqmlplugin_outputDir}"
											LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${quickfutureqmlplugin_outputDir}"
											LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${quickfutureqmlplugin_outputDir}"
											LIBRARY_OUTPUT_DIRECTORY_DEBUG "${quickfutureqmlplugin_outputDir}"
											RUNTIME_OUTPUT_DIRECTORY "${quickfutureqmlplugin_outputDir}"
											RUNTIME_OUTPUT_DIRECTORY_RELEASE "${quickfutureqmlplugin_outputDir}"
											RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${quickfutureqmlplugin_outputDir}"
											RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${quickfutureqmlplugin_outputDir}"
											RUNTIME_OUTPUT_DIRECTORY_DEBUG "${quickfutureqmlplugin_outputDir}"
											)
add_custom_command(TARGET quickfutureqmlplugin POST_BUILD
									 COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/qmldir" "${quickfutureqmlplugin_outputDir}/qmldir"
									 COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/quickfuture.qmltypes" "${quickfutureqmlplugin_outputDir}/quickfuture.qmltypes")
SET(QML_IMPORT_PATH "${CMAKE_CURRENT_BINARY_DIR}" CACHE STRING INTERNAL)
target_link_libraries(quickfutureqmlplugin PRIVATE Qt5::Core Qt5::Qml)
