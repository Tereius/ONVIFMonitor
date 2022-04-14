find_package(CommonLibs QUIET)
if(NOT TARGET onvifcommon)
	find_package(libONVIF REQUIRED)
endif()

if(ANDROID)
        find_package(Qt5 REQUIRED COMPONENTS AndroidExtras)
endif()
find_package(Qt5 REQUIRED COMPONENTS Core Svg Quick Concurrent QuickControls2 QuickTemplates2 OPTIONAL_COMPONENTS QuickCompiler)
find_path(Qt5_plugin_DIR NAMES platforms/qminimald.dll platforms/libqminimald.so platforms/qminimal.dll platforms/libqminimal.so PATH_SUFFIXES "plugins")
find_path(Qt5_qml_DIR NAMES QtQml/qmldir PATH_SUFFIXES "lib" "lib/qml" "qml")

find_package(KF5Kirigami2 REQUIRED COMPONENTS Kirigami2)
find_path(KF5_qml_DIR NAMES org/kde/kirigami.2/qmldir PATH_SUFFIXES "lib" "lib/qml" "qml")

find_package(MDK REQUIRED)

find_library(QtAvLib_PATH NAMES QtAV QtAV QtAVd QtAV1 QtAVd1 QtAV_armeabi-v7a PATH_SUFFIXES "lib")
find_path(QtAvLib_include_DIR NAMES QtAV/QtAV.h PATH_SUFFIXES "include")
find_path(QtAvLib_qml_DIR NAMES QtAV/qmldir PATH_SUFFIXES "lib" "lib/qml" "qml")

find_library(libswresample_PATH NAMES swresample PATH_SUFFIXES "lib")
find_library(libavdevice_PATH NAMES avdevice PATH_SUFFIXES "lib")
find_library(libavfilter_PATH NAMES avfilter PATH_SUFFIXES "lib")
find_library(libavcodec_PATH NAMES avcodec PATH_SUFFIXES "lib")
find_library(libavformat_PATH NAMES avformat PATH_SUFFIXES "lib")
find_library(libswscale_PATH NAMES swscale PATH_SUFFIXES "lib")
find_library(libavutil_PATH NAMES avutil PATH_SUFFIXES "lib")
find_library(libpostproc_PATH NAMES postproc PATH_SUFFIXES "lib")

find_library(OpenSSL_cryptro_PATH NAMES libeay libeay32 libcrypto libcrypto32 eay crypto libcryptod PATH_SUFFIXES "lib")
find_library(OpenSSL_ssl_PATH NAMES ssleay ssleay32 libssl libssl32 eay ssl libssld PATH_SUFFIXES "lib")

find_path(BreezeIcons_DIR NAMES icons/actions/32/application-menu.svg)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(qt_rcc_resources "${PROJECT_SOURCE_DIR}/resources/qt_resources.qrc;${PROJECT_SOURCE_DIR}/resources/quickpromise.qrc;${PROJECT_SOURCE_DIR}/resources/tilesgrid.qrc;${PROJECT_SOURCE_DIR}/resources/default_icons.qrc;${PROJECT_SOURCE_DIR}/resources/fonts.qrc")

set(QML2_IMPORT_PATH "${EXTERNAL_QML2_PATH}")
set(QML_IMPORT_PATH "${EXTERNAL_QML2_PATH}")
set(QT5_ROOT_PATH "${Qt5_DIR}/../../..")
set(QTAV_ROOT_PATH "${Qt5_DIR}/../../..")

include_directories("${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" "${QtAvLib_include_DIR}")

if(MSVC)
	add_definitions(-DNOMINMAX)
	add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

# -------------------------------Home Spy-------------------------------
set(src_files
		LogMessageHandler.cpp
		App.cpp
		main.cpp
		AdvancedQmlApplicationEngine.cpp
		IconImageProvider.cpp
		DeviceDiscoveryModel.cpp
		DeviceManager.cpp
		#Device.cpp
		OnvifDevice.cpp
		GenericDevice.cpp
		AbstractDevice.cpp
		Result.cpp
		DevicesModel.cpp
		Cryptography.cpp
		EventService.cpp
		MediaService.cpp
		#Uuid.cpp
		MediaPlayer.cpp
		MediaProfile.cpp
		MediaProfilesModel.cpp
		#MediaManager.cpp
		ProfileId.cpp
		ImageProvider.cpp
		Timestamp.cpp
		Window.cpp
		Error.cpp
		FutureResult.cpp
		#EventsModel.cpp
		#EventManager.cpp
		#EventHandler.cpp
		#EventBinding.cpp
		#EventSource.cpp
		#EventHandlerModel.cpp
		#EventSourceModel.cpp
		AbstractListModel.cpp
		#EventBindingModel.cpp
		PropertyInfo.cpp
		TopicSetModel.cpp
		OnvifMessageFilterItems.cpp
		SortFilterProxyModel.cpp
		HttpClient.cpp
		MonitorGridModel.cpp
		FrameExtractor.cpp)

# private header
set(header_files
		LogMessageHandler.h
		App.h
		AdvancedQmlApplicationEngine.h
		Util.h
		DeviceInfo.h
		IconImageProvider.h
		DeviceDiscoveryModel.h
		DeviceManager.h
		#Device.h
		AbstractDevice.h
		OnvifDevice.h
		GenericDevice.h
		Result.h
		DevicesModel.h
		Roles.h
		Cryptography.h
		EventService.h
		MediaService.h
		#Uuid.h
		MediaPlayer.h
		MediaProfile.h
		MediaProfilesModel.h
		#MediaManager.h
		ProfileId.h
		ImageProvider.h
		Timestamp.h
		Window.h
		Error.h
		FutureResult.h
		#EventsModel.h
		#EventManager.h
		#EventHandler.h
		#EventBinding.h
		#EventSource.h
		#EventHandlerModel.h
		#EventSourceModel.h
		AbstractListModel.h
		#EventBindingModel.h
		PropertyInfo.h
		TopicSetModel.h
		OnvifMessageFilterItems.h
		SortFilterProxyModel.h
		HttpClient.h
		MonitorGridModel.cpp
		FrameExtractor.h BlockingQueue.h)

add_subdirectory(AsyncFuture)
add_subdirectory(QuickFuture)

#set(ENV{LD_LIBRARY_PATH} "/home/bjoern/.conan/data/ffmpeg/4.0/tereius/stable/package/8446ea3f47151abe755d00523046f28d6549a514/lib")

set(test)
set(BREEZEICONS_DIR ${BreezeIcons_DIR})
kirigami_package_breeze_icons(ICONS test)

set(icons_rcc_file "${PROJECT_SOURCE_DIR}/resources/default_icons.qrc")
file(GLOB icon_files RELATIVE "${PROJECT_SOURCE_DIR}/resources" "${PROJECT_SOURCE_DIR}/resources/icons/onvif/48x48/*.svg")
file(WRITE "${icons_rcc_file}" "<RCC>\n<qresource prefix=\"/\">\n")
file(APPEND "${icons_rcc_file}" "<file>icons/onvif/index.theme</file>\n")
foreach(icon IN LISTS icon_files)
	file(APPEND "${icons_rcc_file}" "<file>${icon}</file>\n")
endforeach(icon)
file(APPEND "${icons_rcc_file}" "</qresource>\n</RCC>")

source_group("Resource Files" FILES ${qt_rcc_resources})


if(Qt5QuickCompiler_DIR)
	qtquick_compiler_add_resources(resSources ${qt_rcc_resources})
else()
	qt5_add_resources(resSources ${qt_rcc_resources})
	message(STATUS "Disabled qml ahead of time compiler")
endif()

if(ANDROID)
	add_library(ONVIFMonitor SHARED ${src_files} ${header_files} ${resSources})
	target_link_libraries(ONVIFMonitor PUBLIC Qt5::AndroidExtras PRIVATE log.so)
else()
	add_executable(ONVIFMonitor WIN32 ${src_files} ${header_files} ${resSources})
endif()

target_compile_definitions(ONVIFMonitor PUBLIC QML_IMPORT_PATHS=$<BUILD_INTERFACE:"${KF5_qml_DIR},${QtAvLib_qml_DIR},${QuickFuture_qml_DIR},${Qt5_qml_DIR}">$<INSTALL_INTERFACE:"">)
target_compile_definitions(ONVIFMonitor PUBLIC QT_PLUGIN_PATHS=$<BUILD_INTERFACE:"${Qt5_plugin_DIR}">$<INSTALL_INTERFACE:"">)
target_compile_definitions(ONVIFMonitor PUBLIC QML_BASE_PATH=$<BUILD_INTERFACE:"${PROJECT_SOURCE_DIR}/resources/gui">$<INSTALL_INTERFACE:"">)
target_compile_features(ONVIFMonitor PUBLIC cxx_strong_enums cxx_auto_type cxx_nullptr cxx_long_long_type cxx_explicit_conversions)
target_link_libraries(ONVIFMonitor PUBLIC onvifcommon PUBLIC onvifdiscovery PUBLIC onvifdevice PUBLIC onvifevent PUBLIC onvifmedia
											PUBLIC ${QtAvLib_PATH} PUBLIC Qt5::Core PUBLIC Qt5::Svg PUBLIC Qt5::Quick PUBLIC Qt5::Concurrent PUBLIC Qt5::QuickControls2
											PUBLIC Qt5::QuickTemplates2 PUBLIC KF5::Kirigami2 PUBLIC mdk
											PUBLIC ${libswresample_PATH} PUBLIC ${libavdevice_PATH} PUBLIC ${libavfilter_PATH} PUBLIC ${libavcodec_PATH}
											PUBLIC ${libavformat_PATH} PUBLIC ${libswscale_PATH} PUBLIC ${libavutil_PATH} PUBLIC ${libpostproc_PATH} PUBLIC quickfutureqmlplugin PRIVATE qtasyncfuture)

add_custom_target(Run-ONVIFMonitor COMMAND export LD_LIBRARY_PATH="/home/bjoern/.conan/data/ffmpeg/4.0/tereius/stable/package/8446ea3f47151abe755d00523046f28d6549a514/lib")
add_dependencies(Run-ONVIFMonitor ONVIFMonitor)

set(QML_IMPORT_PATHS "${KF5_qml_DIR};${CMAKE_CURRENT_SOURCE_DIR};${QML_IMPORT_PATHS}" PARENT_SCOPE)

# add the install target
install(TARGETS ONVIFMonitor RUNTIME DESTINATION bin ARCHIVE DESTINATION lib PUBLIC_HEADER DESTINATION include)

get_filename_component(MDK_FFMPEG_LIBRARY ${MDK_LIBRARY} DIRECTORY)
message(STATUS "--------${MDK_FFMPEG_LIBRARY}")

#include(DeployQt5)
#fixup_qt5_target(ONVIFMonitor)
if(WIN32)
	include(Windeployqt)
	windeployqt(ONVIFMonitor "bin")
elseif(ANDROID)
	include(Androiddeployqt)
	androiddeployqt(ONVIFMonitor PACKAGE "com.github.Tereius.ONVIFMonitor" QML_ROOT_PATH "${PROJECT_SOURCE_DIR}/resources/gui" QML_IMPORT_PATHS "${KF5_qml_DIR};${QtAvLib_qml_DIR};${QuickFuture_qml_DIR}" DEPENDS "${MDK_FFMPEG_LIBRARY}/libffmpeg.so;${MDK_LIBRARY};${OpenSSL_cryptro_PATH};${OpenSSL_ssl_PATH}")
endif()
