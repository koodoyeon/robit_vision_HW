cmake_minimum_required(VERSION 3.5)

project(K-meansclustering VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 및 QtCharts 모듈 찾기
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Charts REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts REQUIRED)

# OpenCV 패키지 찾기
find_package(OpenCV REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

# 실행 파일 설정
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(K-meansclustering
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(K-meansclustering SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(K-meansclustering
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Qt Widgets, Qt Charts, OpenCV 라이브러리 링크
target_link_libraries(K-meansclustering PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts ${OpenCV_LIBS})

set_target_properties(K-meansclustering PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Qt6의 경우 실행 파일 최종 설정
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(K-meansclustering)
endif()
