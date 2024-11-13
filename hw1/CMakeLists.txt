cmake_minimum_required(VERSION 3.5)

project(ransac VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt Widgets 패키지 찾기
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

# 프로젝트 소스 파일 지정
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

# 데이터 파일 경로를 컴파일 정의로 전달
add_compile_definitions(DATA_FILE_PATH="${CMAKE_SOURCE_DIR}/data/First-order function.csv")

# Qt 6과 Qt 5에 따른 실행 파일 설정
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ransac
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(ransac SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(ransac
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Qt 라이브러리 연결
target_link_libraries(ransac PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# 타겟 속성 설정
set_target_properties(ransac PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Qt 6에서는 qt_finalize_executable 호출
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ransac)
endif()
