cmake_minimum_required(VERSION 3.16)

project(Machi_Koro_AI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON) # 自动处理 .ui 文件
set(CMAKE_AUTOMOC ON) # 自动处理 Q_OBJECT 宏
set(CMAKE_AUTORCC ON) # 自动处理 .qrc 资源文件

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES Machi_Koro_AI_zh_CN.ts)

# 定义项目的所有源文件、头文件、UI文件和资源文件
# -----------------------------------------------------------------------------
# 明确列出顶层文件 (不在 src/ 目录下的)
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    # playerchoicedialog.ui # 如果这个文件在根目录，请取消注释
    # Machi_Koro_AI.qrc     # 如果你的资源文件在根目录，请取消注释
    ${TS_FILES}             # 翻译文件
)

# 自动查找 src/ 目录下所有子目录中的 .cpp 和 .h 文件
file(GLOB_RECURSE SRC_RECURSIVE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)
list(APPEND PROJECT_SOURCES ${SRC_RECURSIVE_FILES})

# -----------------------------------------------------------------------------

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Machi_Koro_AI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Machi_Koro_AI APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(Machi_Koro_AI SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Machi_Koro_AI
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(Machi_Koro_AI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# 添加 src 目录到包含路径，以便编译器能找到 src/cards/card.h 等文件
target_include_directories(Machi_Koro_AI PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Machi_Koro_AI)
endif()
set_target_properties(Machi_Koro_AI PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Machi_Koro_AI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Machi_Koro_AI)
endif()
