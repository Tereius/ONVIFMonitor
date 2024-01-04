include(MaterialIcons)

add_subdirectory(AsyncFuture)
add_subdirectory(QuickFuture)

find_package(Qt6 REQUIRED COMPONENTS Core Multimedia Gui Qml Widgets Svg Quick Concurrent QuickControls2 QuickTemplates2)
find_package(MaterialRally REQUIRED)
find_package(QtAppBase REQUIRED)
find_package(QtAppBaseQml REQUIRED)
find_package(libONVIF REQUIRED)
find_package(MDK REQUIRED)

find_library(libswresample_PATH NAMES swresample PATH_SUFFIXES "lib")
find_library(libavdevice_PATH NAMES avdevice PATH_SUFFIXES "lib")
find_library(libavfilter_PATH NAMES avfilter PATH_SUFFIXES "lib")
find_library(libavcodec_PATH NAMES avcodec PATH_SUFFIXES "lib")
find_library(libavformat_PATH NAMES avformat PATH_SUFFIXES "lib")
find_library(libswscale_PATH NAMES swscale PATH_SUFFIXES "lib")
find_library(libavutil_PATH NAMES avutil PATH_SUFFIXES "lib")
find_library(libpostproc_PATH NAMES postproc PATH_SUFFIXES "lib")
find_path(libav_include_PATH libavformat/avformat.h)

find_library(OpenSSL_cryptro_PATH NAMES libeay libeay32 libcrypto libcrypto32 eay crypto libcryptod PATH_SUFFIXES "lib")
find_library(OpenSSL_ssl_PATH NAMES ssleay ssleay32 libssl libssl32 eay ssl libssld PATH_SUFFIXES "lib")

include_directories("${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" "${libav_include_PATH}")

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_definitions(-DNOMINMAX)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Werror=return-type)
endif ()

# -------------------------------Home Spy-------------------------------
set(src_files
        #LogMessageHandler.cpp
        App.cpp
        #AdvancedQmlApplicationEngine.cpp
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
        OnvifRtspClient.cpp
        MonitorGridModel.cpp
        #FrameExtractor.cpp
        Serializable.cpp
)

# private header
set(header_files
        #LogMessageHandler.h
        App.h
        #AdvancedQmlApplicationEngine.h
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
        OnvifRtspClient.h
        #FrameExtractor.h
        BlockingQueue.h
        Serializable.h
)

qt_add_executable(ONVIFMonitor
        main.cpp
        App.h
        App.cpp
        BackgroundService.h
        BackgroundService.cpp
        BlockingQueue.h
        Util.h
        DeviceInfo.h
        IconImageProvider.h
        IconImageProvider.cpp
        OnvifDevice.h
        OnvifDevice.cpp
        GenericDevice.h
        GenericDevice.cpp
        AbstractDevice.h
        AbstractDevice.cpp
        Result.h
        Result.cpp
        Cryptography.h
        Cryptography.cpp
        EventService.h
        EventService.cpp
        MediaService.h
        MediaService.cpp
        MediaProfile.h
        MediaProfile.cpp
        ImageProvider.h
        ImageProvider.cpp
        ProfileId.h
        ProfileId.cpp
        Timestamp.h
        Timestamp.cpp
        Window.h
        Window.cpp
        Error.h
        Error.cpp
        FutureResult.h
        FutureResult.cpp
        AbstractListModel.h
        AbstractListModel.cpp
        TopicSetModel.h
        TopicSetModel.cpp
        OnvifMessageFilterItems.h
        OnvifMessageFilterItems.cpp
        HttpClient.h
        HttpClient.cpp
        OnvifRtspClient.h
        OnvifRtspClient.cpp
        RtspMessage.h
        RtspMessage.cpp
        SessionDescription.h
        SessionDescription.cpp
        RtspStream.cpp
        RtspStream.h
        Serializable.h
        Serializable.cpp
)

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

qt_add_qml_module(ONVIFMonitor
        URI Onvif

        SOURCES
        SortFilterProxyModel.h
        SortFilterProxyModel.cpp
        DeviceDiscoveryModel.h
        DeviceDiscoveryModel.cpp
        DevicesModel.h
        DevicesModel.cpp
        DeviceManager.h
        DeviceManager.cpp
        MediaProfilesModel.h
        MediaProfilesModel.cpp
        MediaPlayer.h
        MediaPlayer.cpp
        MonitorGridModel.h
        MonitorGridModel.cpp
        PropertyInfo.h
        PropertyInfo.cpp
        Roles.h
        Roles.cpp
        Onvif.h
        Onvif.cpp
        MicrophoneRtpSource.h
        MicrophoneRtpSource.cpp

        QML_FILES
        Onvif/pages/EditMonitorPage.qml
        Onvif/controls/Placeholder.qml
        Onvif/dialogs/AddMonitorDialog.qml
        Onvif/dialogs/EditMonitorDialog.qml
        Onvif/MediaProfilesListView.qml
        Onvif/AudioDeviceComboBox.qml
        Onvif/AboutPage.qml
        Onvif/BusyCombobox.qml
        Onvif/BusyPopup.qml
        Onvif/CameraImage.qml
        Onvif/CameraStream.qml
        Onvif/CredentialsDialog.qml
        Onvif/DesktopExampleApp.qml
        Onvif/DeviceInfoDialog.qml
        Onvif/DeviceInfoPage.qml
        Onvif/DevicePage.qml
        Onvif/DeviceSettingsDialog.qml
        Onvif/DevicesListView.qml
        Onvif/DevicesPage.qml
        Onvif/EventHandlerSettingsDialog.qml
        Onvif/EventPropertiesDialog.qml
        Onvif/EventServiceInfoPage.qml
        Onvif/EventSettingsPage.qml
        Onvif/ExampleApp.qml
        Onvif/ExpandableListView.qml
        Onvif/FixedSizeGridLayout.qml
        Onvif/FormLayout.qml
        Onvif/Icon.qml
        Onvif/ListItem.qml
        Onvif/MediaProfilesDevicesSettingsPage.qml
        Onvif/MediaProfileSettingsPage.qml
        Onvif/MediaProfilesListPage.qml
        Onvif/MediaServiceInfoPage.qml
        Onvif/MonitoringPage.qml
        Onvif/NewDevicePage.qml
        Onvif/NewEventBindingDialog.qml
        Onvif/OnvifMessageFilterBuilder.qml
        Onvif/PingAnimation.qml
        Onvif/PlaceholderPage.qml
        Onvif/main.qml
        Onvif/SettingsPage.qml
        Onvif/Style.qml
        Onvif/VideoWallPane.qml
        Onvif/WRoundButton.qml
        Onvif/dialogs/EditOnvifDeviceDialog.qml
        Onvif/FilterBuilder/BaseFilterComponent.qml
        Onvif/FilterBuilder/DropLine.qml
        Onvif/SwipePage.qml
)

qt_add_qml_module(TilesGrid
        URI TilesGrid

        QML_FILES
        TilesGrid/Tile.qml
        TilesGrid/TilesGrid.qml
        TilesGrid/TilesGridData.qml
        TilesGrid/TilesGridLayout.qml
        TilesGrid/TilesGridLogic.qml
        TilesGrid/UndoStack.qml
)

#target_compile_features(ONVIFMonitor PUBLIC cxx_strong_enums cxx_auto_type cxx_nullptr cxx_long_long_type cxx_explicit_conversions)
target_link_libraries(ONVIFMonitor
        PRIVATE mdk
        PRIVATE QtAppBase
        PRIVATE TilesGrid
        PRIVATE quickfutureqmlplugin
        PRIVATE onvifcommon
        PRIVATE onvifdiscovery
        PRIVATE onvifdevice
        PRIVATE onvifevent
        PRIVATE onvifmedia
        PRIVATE onvifmedia2
        PRIVATE onvifprobe
        PRIVATE qtasyncfuture
        PRIVATE Qt6::Core
        PRIVATE Qt6::Multimedia
        PRIVATE Qt6::Gui
        PRIVATE Qt6::Widgets
        PRIVATE Qt6::Svg
        PRIVATE Qt6::Qml
        PRIVATE Qt6::Quick
        PRIVATE Qt6::Concurrent
        PRIVATE Qt6::QuickControls2
        PRIVATE Qt6::QuickTemplates2
        PRIVATE ${libswresample_PATH}
        PRIVATE ${libavdevice_PATH}
        PRIVATE ${libavfilter_PATH}
        PRIVATE ${libavcodec_PATH}
        PRIVATE ${libavformat_PATH}
        PRIVATE ${libswscale_PATH}
        PRIVATE ${libavutil_PATH}
        PRIVATE ${libpostproc_PATH}
)

if (ANDROID)
    target_link_libraries(ONVIFMonitor PRIVATE Qt6::CorePrivate)
endif ()

add_material_icon_theme_resource(ONVIFMonitor THEME_NAME material ICONS
        chevron-right
        chevron-left
        cctv # camera icon
        cctv-off # camera icon
        refresh
        delete
        content-save
        alert
        harddisk
        harddisk-remove
        lock-smart
        plus
        check
        cog
        information
        new-box
        close
        link
        play
        camera-wireless
        web-off # device offline
        microphone
        bullhorn
        monitor
        monitor-off
        speaker
        flip-horizontal
        flip-vertical
        pencil # edit button
)

function(extractSoName PATH soname)
    execute_process(COMMAND ${CMAKE_OBJDUMP} -p ${PATH} COMMAND grep SONAME OUTPUT_VARIABLE objdump_data OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX MATCHALL "[ ]*SONAME[ ]+(.+)" tmp ${objdump_data})
    set(${soname} "${CMAKE_MATCH_1}" PARENT_SCOPE)
endfunction()

extractSoName("${libswresample_PATH}" libswresample_NAME)
extractSoName("${libavdevice_PATH}" libavdevice_NAME)
extractSoName("${libavfilter_PATH}" libavfilter_NAME)
extractSoName("${libavcodec_PATH}" libavcodec_NAME)
extractSoName("${libavformat_PATH}" libavformat_NAME)
extractSoName("${libswscale_PATH}" libswscale_NAME)
extractSoName("${libavutil_PATH}" libavutil_NAME)

target_compile_definitions(ONVIFMonitor PRIVATE LIB_SWRESAMPLE_NAME="${libswresample_NAME}")
target_compile_definitions(ONVIFMonitor PRIVATE LIB_AVDEVICE_NAME="${libavdevice_NAME}")
target_compile_definitions(ONVIFMonitor PRIVATE LIB_AVFILTER_NAME="${libavfilter_NAME}")
target_compile_definitions(ONVIFMonitor PRIVATE LIB_AVCODEC_NAME="${libavcodec_NAME}")
target_compile_definitions(ONVIFMonitor PRIVATE LIB_AVFORMAT_NAME="${libavformat_NAME}")
target_compile_definitions(ONVIFMonitor PRIVATE LIB_SWSCALE_NAME="${libswscale_NAME}")
target_compile_definitions(ONVIFMonitor PRIVATE LIB_AVUTIL_NAME="${libavutil_NAME}")

set_target_properties(ONVIFMonitor PROPERTIES QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/android")

set_property(TARGET ONVIFMonitor APPEND PROPERTY QT_ANDROID_EXTRA_LIBS "${libswresample_PATH}")
set_property(TARGET ONVIFMonitor APPEND PROPERTY QT_ANDROID_EXTRA_LIBS "${libavdevice_PATH}")
set_property(TARGET ONVIFMonitor APPEND PROPERTY QT_ANDROID_EXTRA_LIBS "${libavfilter_PATH}")
set_property(TARGET ONVIFMonitor APPEND PROPERTY QT_ANDROID_EXTRA_LIBS "${libavcodec_PATH}")
set_property(TARGET ONVIFMonitor APPEND PROPERTY QT_ANDROID_EXTRA_LIBS "${libavformat_PATH}")
set_property(TARGET ONVIFMonitor APPEND PROPERTY QT_ANDROID_EXTRA_LIBS "${libswscale_PATH}")
set_property(TARGET ONVIFMonitor APPEND PROPERTY QT_ANDROID_EXTRA_LIBS "${libavutil_PATH}")
set_property(TARGET ONVIFMonitor APPEND PROPERTY QT_ANDROID_EXTRA_LIBS "${libpostproc_PATH}")

set_property(TARGET ONVIFMonitor APPEND PROPERTY QT_ANDROID_EXTRA_LIBS "${OpenSSL_cryptro_PATH}")
set_property(TARGET ONVIFMonitor APPEND PROPERTY QT_ANDROID_EXTRA_LIBS "${OpenSSL_ssl_PATH}")

target_link_qml_module(ONVIFMonitor PRIVATE MaterialRally)
target_link_qml_module(ONVIFMonitor PRIVATE QtAppBaseQml)

# add the install target
install_app(ONVIFMonitor)
