cmake_minimum_required(VERSION 3.16)

project(Machi_Koro_AI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES Machi_Koro_AI_zh_CN.ts)

# 定义项目的所有源文件、头文件、UI文件和资源文件
# -----------------------------------------------------------------------------
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui             # .ui 文件在根目录
    #playerchoicedialog.ui     # .ui 文件在根目录
    # Machi_Koro_AI.qrc       # 暂时注释掉，待资源文件创建后再取消注释
    ${TS_FILES}               # 翻译文件
)

# 添加 src/ 目录下直接的源文件和头文件 (game.h/cpp, player.h/cpp, global.h)
file(GLOB SRC_ROOT_HEADERS "src/*.h")
file(GLOB SRC_ROOT_SOURCES "src/*.cpp")
list(APPEND PROJECT_SOURCES ${SRC_ROOT_HEADERS} ${SRC_ROOT_SOURCES})

# 添加卡牌定义源文件和头文件 (仍在 src/cards/)
file(GLOB CARDS_HEADERS "src/cards/*.h")
file(GLOB CARDS_SOURCES "src/cards/*.cpp")
list(APPEND PROJECT_SOURCES ${CARDS_HEADERS} ${CARDS_SOURCES})

# 添加命令模式相关源文件和头文件 (仍在 src/commands/)
file(GLOB COMMANDS_HEADERS "src/commands/*.h")
file(GLOB COMMANDS_SOURCES "src/commands/*.cpp")
list(APPEND PROJECT_SOURCES ${COMMANDS_HEADERS} ${COMMANDS_SOURCES})

# -----------------------------------------------------------------------------

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Machi_Koro_AI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}

        src/player.h src/player.cpp

        src/cardfactory.h src/cardfactory.cpp
        src/cardstore.h src/cardstore.cpp
        src/cards/card.cpp
        src/game.h src/game.cpp
        src/commands/gamecommand.h src/commands/gamecommand.cpp
        src/dice.h src/dice.cpp
        src/randomutils.h
        src/randomutils.h
        src/cards/blue/wheatfield.h src/cards/blue/wheatfield.cpp
        src/cards/blue/ranch.h src/cards/blue/ranch.cpp
        src/cards/blue/flowerorchard.h src/cards/blue/flowerorchard.cpp
        src/cards/blue/forest.h src/cards/blue/forest.cpp
        src/cards/blue/mackerelboat.h src/cards/blue/mackerelboat.cpp
        src/cards/blue/appleorchard.h src/cards/blue/appleorchard.cpp
        src/cards/red/sushibar.h src/cards/red/sushibar.cpp
        src/cards/red/cafe.h src/cards/red/cafe.cpp
        src/cards/red/frenchrestaurant.h src/cards/red/frenchrestaurant.cpp
        src/cards/red/pizzajoint.h src/cards/red/pizzajoint.cpp
        src/cards/red/familyrestaurant.h src/cards/red/familyrestaurant.cpp
        src/cards/red/membersonlyclub.h src/cards/red/membersonlyclub.cpp
        src/cards/green/generalstore.h src/cards/green/generalstore.cpp
        src/cards/green/bakery.h src/cards/green/bakery.cpp
        src/cards/green/flowershop.h src/cards/green/flowershop.cpp
        src/cards/green/cheesefactory.h src/cards/green/cheesefactory.cpp
        src/cards/green/furniturefactory.h src/cards/green/furniturefactory.cpp
        src/cards/green/sodabottlingplant.h src/cards/green/sodabottlingplant.cpp
        src/cards/green/fruitandvegetablemarket.h src/cards/green/fruitandvegetablemarket.cpp
        src/cards/purple/stadium.h src/cards/purple/stadium.cpp
        src/cards/purple/publisher.h src/cards/purple/publisher.cpp
        src/cards/purple/taxoffice.h src/cards/purple/taxoffice.cpp
        src/cards/purple/techstartup.h src/cards/purple/techstartup.cpp
        src/cards/landmark/trainstation.h src/cards/landmark/trainstation.cpp
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

# 添加 src 目录到包含路径，以便编译器能找到 global.h、game.h 等
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
