include(MaterialIcons)

add_subdirectory(AsyncFuture)
add_subdirectory(QuickFuture)

find_package(Qt6 REQUIRED COMPONENTS Core Multimedia Gui Qml Widgets Svg Quick Concurrent QuickControls2 QuickTemplates2 RemoteObjects)
find_package(MaterialRally REQUIRED)
find_package(QtAppBase REQUIRED)
find_package(QtAppBaseQml REQUIRED)
find_package(OpenSSL REQUIRED) # needed by libONVIF cmake config file
find_package(libONVIF REQUIRED)
find_package(mdk-sdk REQUIRED)

include_directories("${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_definitions(-DNOMINMAX)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Werror=return-type)
endif ()

# -------------------------------ONVIFmonitor-------------------------------

qt_add_executable(ONVIFMonitor
        main.cpp
        App.h
        App.cpp
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
        EventsModel.h
        EventsModel.cpp
        EventMessage.h
        EventMessage.cpp
)

qt_add_repc_sources(ONVIFMonitor DeviceManager.rep)
qt_add_repc_replicas(ONVIFMonitor EventManager.rep)

qt_add_executable(ONVIFMonitorService
        mainService.cpp
        Util.h
        BackgroundService.h
        BackgroundService.cpp
        EventManager.h
        EventManager.cpp
        EventBinding.h
        EventBinding.cpp
        EventSource.h
        EventSource.cpp
        EventHandler.h
        EventHandler.cpp
        PropertyInfo.h
        ProfileId.cpp
        Result.h
        Result.cpp
        EventMessage.h
        EventMessage.cpp
)

add_dependencies(ONVIFMonitor ONVIFMonitorService)

qt_add_repc_sources(ONVIFMonitorService EventManager.rep)
qt_add_repc_replicas(ONVIFMonitorService DeviceManager.rep)

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

qt_add_qml_module(ONVIFMonitor
        URI Onvif

        SOURCES
        DeviceManager.h
        DeviceManager.cpp
        SortFilterProxyModel.h
        SortFilterProxyModel.cpp
        DeviceDiscoveryModel.h
        DeviceDiscoveryModel.cpp
        DevicesModel.h
        DevicesModel.cpp
        MediaProfilesModel.h
        MediaProfilesModel.cpp
        MediaPlayer.h
        MediaPlayer.cpp
        MonitorGridModel.h
        MonitorGridModel.cpp
        PropertyInfo.h
        PropertyInfo.cpp
        Enums.h
        Onvif.h
        Onvif.cpp
        MicrophoneRtpSource.h
        MicrophoneRtpSource.cpp

        QML_FILES
        Onvif/pages/EditMonitorPage.qml
        Onvif/pages/MonitoringSwipePage.qml
        Onvif/controls/Placeholder.qml
        Onvif/controls/VolumeDial.qml
        Onvif/controls/CameraLoadingIndicator.qml
        Onvif/dialogs/AddMonitorDialog.qml
        Onvif/dialogs/EditMonitorDialog.qml
        Onvif/MediaProfilesListView.qml
        Onvif/controls/AudioDeviceComboBox.qml
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
        PRIVATE mdk-sdk::mdk-sdk
        PRIVATE MDK::mdk
        PRIVATE MDK::ffmpeg
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
        PRIVATE Qt6::RemoteObjects
)

target_link_libraries(ONVIFMonitorService
        PRIVATE QtAppBase
        PRIVATE onvifcommon
        PRIVATE onvifdiscovery
        PRIVATE onvifdevice
        PRIVATE onvifevent
        PRIVATE onvifmedia
        PRIVATE onvifmedia2
        PRIVATE onvifprobe
        PRIVATE qtasyncfuture
        PRIVATE Qt6::Core
        PRIVATE Qt6::Concurrent
        PRIVATE Qt6::RemoteObjects
)

if (ANDROID)
    set_property(TARGET ONVIFMonitorService PROPERTY _qt_is_android_executable FALSE) # hidden option to skip apk target creation
    target_link_libraries(ONVIFMonitor PRIVATE Qt6::CorePrivate)
    target_link_libraries(ONVIFMonitorService PRIVATE Qt6::CorePrivate)
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
        alert-outline
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
        eye
        eye-off
)

function(extractSoName PATH soname)
    execute_process(COMMAND ${CMAKE_OBJDUMP} -p ${PATH} COMMAND grep SONAME OUTPUT_VARIABLE objdump_data OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX MATCHALL "[ ]*SONAME[ ]+(.+)" tmp ${objdump_data})
    set(${soname} "${CMAKE_MATCH_1}" PARENT_SCOPE)
endfunction()

set_target_properties(ONVIFMonitor PROPERTIES QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/android")

set_property(TARGET ONVIFMonitor APPEND PROPERTY QT_ANDROID_EXTRA_LIBS "${CMAKE_CURRENT_BINARY_DIR}/libONVIFMonitorService_${ANDROID_ABI}.so")

#set_property(TARGET ONVIFMonitor APPEND PROPERTY QT_ANDROID_EXTRA_LIBS "${OpenSSL_cryptro_PATH}")
#set_property(TARGET ONVIFMonitor APPEND PROPERTY QT_ANDROID_EXTRA_LIBS "${OpenSSL_ssl_PATH}")

target_link_qml_module(ONVIFMonitor PRIVATE MaterialRally)
target_link_qml_module(ONVIFMonitor PRIVATE QtAppBaseQml)

register_icon(ONVIFMonitor
        48x48 "${CMAKE_SOURCE_DIR}/resources/icons/ONVIFmonitor48.png"
)

# add the install target
install_app(ONVIFMonitor)
