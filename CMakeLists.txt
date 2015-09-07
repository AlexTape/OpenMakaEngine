cmake_minimum_required(VERSION 3.2)
project(OpenMakaEngine)

find_package(OpenCV REQUIRED )
find_package(OpenGL REQUIRED )

include_directories(${OpenCV_INCLUDE_DIR})
include_directories(${OpenGL_INCLUDE_DIR})

link_directories(${OpenCV_LIBRARY_DIR})
link_directories(${OpenGL_LIBRARY_DIR})

#add_subdirectory(src/akaze)
#IF(UNIX)
#    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wno-unknown-pragmas -Wunused-variable -g -msse2 -msse3 -msse4 -ffast-math")
#    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wno-unknown-pragmas -Wunused-variable -g -msse2 -msse3 -msse4 -ffast-math")
#ENDIF(UNIX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

## To get much more output in your console uncomment this:
add_definitions(-D_DEBUG_)

set(SOURCE_FILES
    src/App.cpp
    src/Application/Interfaces/OpenMakaEngine.h
    src/Application/Recognition/Analyzer.cpp
    src/Application/Recognition/Analyzer.h
    src/Application/Controller.cpp
    src/Application/Controller.h
    src/Application/Tracker/Tracker.cpp
    src/Application/Tracker/Tracker.h
    src/Application/Recognition/SceneFrame.cpp
    src/Application/Recognition/SceneFrame.h
    src/Application/Recognition/ObjectPattern.cpp
    src/Application/Recognition/ObjectPattern.h
    src/Application/Helper/Timer.cpp
    src/Application/Helper/Timer.h
    src/Application/Helper/Drawer.cpp
    src/Application/Helper/Drawer.h
    src/Application/Helper/Statistics.cpp
    src/Application/Helper/Statistics.h

    # AKAZE from https://github.com/thorikawa/akaze-opencv
    src/Application/akaze/akaze_features.cpp
    src/Application/akaze/akaze_features.h
    src/Application/akaze/akaze_fed.cpp
    src/Application/akaze/akaze_fed.h
    src/Application/akaze/akaze_nldiffusion_functions.cpp
    src/Application/akaze/akaze_nldiffusion_functions.h
    src/Application/akaze/akaze_utils.cpp
    src/Application/akaze/akaze_utils.h
    src/Application/akaze/AKAZE.cpp
    src/Application/akaze/AKAZE.h
)

add_executable(OpenMakaEngine ${SOURCE_FILES} )

target_link_libraries( OpenMakaEngine ${OpenCV_LIBRARIES} )
target_link_libraries( OpenMakaEngine ${OpenGL_LIBRARIES} )
