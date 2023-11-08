find_package(Qt6 COMPONENTS Core Qml REQUIRED)

set(quickfutureqmlplugin_outputDir "${CMAKE_CURRENT_BINARY_DIR}/QuickFutureBuildDir/QuickFuture")

add_library(quickfutureqmlplugin SHARED qffuture.h qfvariantwrapper.h quickfuture.h QuickFuture qffuture.cpp)

#target_compile_definitions(quickfutureqmlplugin PUBLIC QUICKFUTURE_QML_PATH="${CMAKE_CURRENT_BINARY_DIR}" PRIVATE QUICK_FUTURE_BUILD_PLUGIN)

#set_target_properties(quickfutureqmlplugin
#											PROPERTIES
#											OUTPUT_NAME "quickfutureqmlplugin"
#											LIBRARY_OUTPUT_DIRECTORY "${quickfutureqmlplugin_outputDir}"
#											LIBRARY_OUTPUT_DIRECTORY_RELEASE "${quickfutureqmlplugin_outputDir}"
#											LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${quickfutureqmlplugin_outputDir}"
#											LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${quickfutureqmlplugin_outputDir}"
#											LIBRARY_OUTPUT_DIRECTORY_DEBUG "${quickfutureqmlplugin_outputDir}"
#											RUNTIME_OUTPUT_DIRECTORY "${quickfutureqmlplugin_outputDir}"
#											RUNTIME_OUTPUT_DIRECTORY_RELEASE "${quickfutureqmlplugin_outputDir}"
##											RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${quickfutureqmlplugin_outputDir}"
	#										RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${quickfutureqmlplugin_outputDir}"
	#										RUNTIME_OUTPUT_DIRECTORY_DEBUG "${quickfutureqmlplugin_outputDir}"
	#										)

set_target_properties(quickfutureqmlplugin PROPERTIES PUBLIC_HEADER "quickfuture.h")

target_include_directories(quickfutureqmlplugin INTERFACE
													 $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
													 $<INSTALL_INTERFACE:include>)

add_custom_command(TARGET quickfutureqmlplugin POST_BUILD
									 COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/qmldir" "${quickfutureqmlplugin_outputDir}/qmldir"
									 COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/quickfuture.qmltypes" "${quickfutureqmlplugin_outputDir}/quickfuture.qmltypes")

target_link_libraries(quickfutureqmlplugin PRIVATE Qt6::Core Qt6::Qml)
