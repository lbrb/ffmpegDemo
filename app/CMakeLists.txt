# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             play-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/video.cpp
             src/main/cpp/audio.cpp
             src/main/cpp/player.cpp
             )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

set(main-dir ${CMAKE_SOURCE_DIR}/src/main)

set(ANDROID_ABI armeabi-v7a)

add_library( libavutil
             SHARED
             IMPORTED
             GLOBAL
             )

set_target_properties( # Specifies the target library.
                       libavutil

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${main-dir}/jniLibs/${ANDROID_ABI}/libavutil-54.so )

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )


add_library( libavcodec-56
             SHARED
             IMPORTED
             )

set_target_properties( # Specifies the target library.
                       libavcodec-56

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${main-dir}/jniLibs/${ANDROID_ABI}/libavcodec-56.so )
add_library( libavdevice-56
             SHARED
             IMPORTED
             )

set_target_properties( # Specifies the target library.
                       libavdevice-56

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${main-dir}/jniLibs/${ANDROID_ABI}/libavdevice-56.so )

add_library( libavfilter-5
             SHARED
             IMPORTED
             )

set_target_properties( # Specifies the target library.
                       libavfilter-5

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${main-dir}/jniLibs/${ANDROID_ABI}/libavfilter-5.so )

add_library( libavformat-56
             SHARED
             IMPORTED
             )

set_target_properties( # Specifies the target library.
                       libavformat-56

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${main-dir}/jniLibs/${ANDROID_ABI}/libavformat-56.so )



add_library( libpostproc-53
             SHARED
             IMPORTED
             )

set_target_properties( # Specifies the target library.
                       libpostproc-53

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${main-dir}/jniLibs/${ANDROID_ABI}/libpostproc-53.so )


add_library( libswresample-1
             SHARED
             IMPORTED
             )

set_target_properties( # Specifies the target library.
                       libswresample-1

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${main-dir}/jniLibs/${ANDROID_ABI}/libswresample-1.so )



add_library( libswscale-3
             SHARED
             IMPORTED
             )

set_target_properties( # Specifies the target library.
                      libswscale-3

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${main-dir}/jniLibs/${ANDROID_ABI}/libswscale-3.so )


add_library( yuvutil
             SHARED
             IMPORTED
             )

set_target_properties( # Specifies the target library.
                      yuvutil

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${main-dir}/jniLibs/${ANDROID_ABI}/libyuvutil.so )

add_library( yuv
             SHARED
             IMPORTED
             )

set_target_properties( # Specifies the target library.
                      yuv

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${main-dir}/jniLibs/${ANDROID_ABI}/libyuv.so )



include_directories(${main-dir}/cpp/inc/ffmpeg)
include_directories(${main-dir}/cpp/inc/yuv)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.


target_link_libraries( # Specifies the target library.
                        play-lib

                        yuv
                        yuvutil

                        libavformat-56
                        libavcodec-56
                        libavdevice-56
                        libavfilter-5
                        libpostproc-53
                        libswresample-1
                        libswscale-3
                        libavutil

                         -ljnigraphics
                         -landroid
                       # Links the target library to the log library
                       # included in the NDK.

                       ${log-lib} )