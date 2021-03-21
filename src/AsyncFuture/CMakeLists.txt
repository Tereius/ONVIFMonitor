add_library(qtasyncfuture INTERFACE)

target_include_directories(qtasyncfuture INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

set_target_properties(qtasyncfuture PROPERTIES PUBLIC_HEADER "asyncfuture.h")
