find_package(CommonLibs QUIET)
find_package(libONVIF REQUIRED)
find_package(Qt5 COMPONENTS Core Svg Quick Concurrent QuickControls2 REQUIRED)
find_path(Qt5_qml_DIR NAMES QtQml/qmldir PATH_SUFFIXES "lib" "lib/qml" "qml")
find_path(Qt5_plugin_DIR NAMES platforms/qminimald.dll platforms/qminimal.dll PATH_SUFFIXES "plugins")
find_library(QtAvLib_PATH NAMES QtAV QtAVd QtAV1 QtAVd1 PATH_SUFFIXES "lib")
find_path(QtAvLib_include_DIR NAMES QtAV/QtAV.h PATH_SUFFIXES "include")
find_path(QtAvLib_qml_DIR NAMES QtAV/qmldir PATH_SUFFIXES "lib" "lib/qml" "qml")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(qt_rcc_resources "${PROJECT_SOURCE_DIR}/resources/qt_resources.qrc;${PROJECT_SOURCE_DIR}/resources/default_icons.qrc")

set(QML2_IMPORT_PATH "${EXTERNAL_QML2_PATH}")
set(QML_IMPORT_PATH "${EXTERNAL_QML2_PATH}")
set(QT5_ROOT_PATH "${Qt5_DIR}/../../..")
set(QTAV_ROOT_PATH "${Qt5_DIR}/../../..")
# Configure a header file to pass some of the CMake settings to the source code.
configure_file("${PROJECT_SOURCE_DIR}/src/info.in" "${PROJECT_BINARY_DIR}/src/info.h")

include_directories("${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" "${QtAvLib_include_DIR}")

if(MSVC)
	add_definitions(-DNOMINMAX)
	add_definitions(-DQT_QML_DEBUG)
	add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

# -------------------------------Home Spy-------------------------------
set(src_files
	main.cpp
	DeviceInfo.cpp
	IconImageProvider.cpp
	DeviceDiscoveryModel.cpp
	DeviceManager.cpp
	Device.cpp
	Result.cpp
	DevicesModel.cpp
	Cryptography.cpp
	EventService.cpp
	MediaService.cpp
	Uuid.cpp
	MediaProfile.cpp
	MediaProfilesModel.cpp
	MediaManager.cpp
	ProfileId.cpp
	ImageProvider.cpp
	Timestamp.cpp
	Window.cpp
	Error.cpp
	FutureResult.cpp
	EventsModel.cpp
	EventManager.cpp
	EventHandler.cpp
	EventBinding.cpp
	EventSource.cpp
	EventHandlerModel.cpp
	EventSourceModel.cpp
	AbstractListModel.cpp
	EventBindingModel.cpp
	PropertyInfo.cpp
	TopicSetModel.cpp
	OnvifMessageFilterItems.cpp
)

# private header
set(header_files
	DeviceInfo.h
	IconImageProvider.h
	DeviceDiscoveryModel.h
	DeviceManager.h
	Device.h
	Result.h
	DevicesModel.h
	Roles.h
	Cryptography.h
	EventService.h
	MediaService.h
	Uuid.h
	MediaProfile.h
	MediaProfilesModel.h
	MediaManager.h
	ProfileId.h
	ImageProvider.h
	Timestamp.h
	Window.h
	Error.h
	FutureResult.h
	EventsModel.h
	EventManager.h
	EventHandler.h
	EventBinding.h
	EventSource.h
	EventHandlerModel.h
	EventSourceModel.h
	AbstractListModel.h
	EventBindingModel.h
	PropertyInfo.h
	TopicSetModel.h
	OnvifMessageFilterItems.h
)

source_group("Resource Files" FILES ${qt_rcc_resources})

qt5_add_resources(resSources ${qt_rcc_resources})
add_executable(HomeSpy WIN32 ${src_files} ${header_files} ${resSources})
target_link_libraries(HomeSpy onvifcommon onvifdiscovery onvifdevice onvifevent onvifmedia ${QtAvLib_PATH} Qt5::Core Qt5::Svg Qt5::Quick Qt5::Concurrent Qt5::QuickControls2)

# add the install target
install(TARGETS HomeSpy RUNTIME DESTINATION bin ARCHIVE DESTINATION lib PUBLIC_HEADER DESTINATION include)

#include(DeployQt5)
#fixup_qt5_target(HomeSpy)
include(Windeployqt)
windeployqt(HomeSpy "bin")
