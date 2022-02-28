find_library(libffmpeg_PATH NAMES ffmpeg PATH_SUFFIXES "lib")
find_library(libcmdutils_PATH NAMES cmdutils PATH_SUFFIXES "lib")
find_path(libffmpeg_include_DIR NAMES libffmpeg/ffmpeg.h PATH_SUFFIXES "include")


add_executable(test main.cpp)
target_link_libraries(test PRIVATE ${libffmpeg_PATH} PUBLIC ${libswresample_PATH} PUBLIC ${libavdevice_PATH} PUBLIC ${libavfilter_PATH} PUBLIC ${libavcodec_PATH}
											PUBLIC ${libavformat_PATH} PUBLIC ${libswscale_PATH} PUBLIC ${libavutil_PATH} PUBLIC ${libpostproc_PATH})
target_include_directories(test PRIVATE ${libffmpeg_include_DIR})
