# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

set(codec_dir codec)

include_directories(include
        codec camera opengl)

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
        native_bridge.cpp
        ${codec_dir}/H264Encoder.cpp
        ${codec_dir}/G711Encoder.cpp
        camera/CameraEngine.cpp
        camera/ManagerCamera.cpp
        include/CameraUtil.cpp
        camera/CameraListener.cpp
        opengl/EGLHelper.cpp
        opengl/CodecInputSurface.cpp
        )



target_link_libraries(native-lib
                      mediandk
                      log
                      android
        camera2ndk
        EGL
        GLESv2)