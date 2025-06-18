# 指定 CMake 的最低版本要求。Qt6 通常需要 CMake 3.16 或更高版本。
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# 定义项目名称、版本和使用的语言。
project(Machi_Koro_AI VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准为 C++17，并要求编译器必须支持此标准。
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# 启用 Qt 的自动处理功能：
# CMAKE_AUTOMOC: 自动为 Q_OBJECT 类运行 moc (Meta-Object Compiler)。
# CMAKE_AUTORCC: 自动编译 .qrc 资源文件。
# CMAKE_AUTOUIC: 自动编译 .ui UI 定义文件。
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找 Qt6 库。这里指定了 Widgets、Core 和 Gui 组件，这些是桌面应用常用的。
# REQUIRED 表示如果找不到这些组件，CMake 配置将失败。
find_package(Qt6 COMPONENTS Widgets Core Gui LinguistTools REQUIRED)

# 显式收集所有源代码文件。
# 请根据你的实际 .cpp 文件列表进行核对和补充。
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    src/card.cpp
    src/cardfactory.cpp
    src/cardstore.cpp
    src/commandfactory.cpp
    src/dice.cpp
    src/gamecommand.cpp
    src/gamecontroller.cpp
    src/gamestate.cpp
    src/player.cpp

    src/cards/blue/appleorchard.cpp
    src/cards/blue/flowerorchard.cpp
    src/cards/blue/forest.cpp
    src/cards/blue/mackerelboat.cpp
    src/cards/blue/ranch.cpp
    src/cards/blue/wheatfield.cpp

    src/cards/red/cafe.cpp
    src/cards/red/familyrestaurant.cpp
    src/cards/red/frenchrestaurant.cpp
    src/cards/red/membersonlyclub.cpp
    src/cards/red/pizzajoint.cpp
    src/cards/red/sushibar.cpp

    src/cards/green/bakery.cpp
    src/cards/green/cheesefactory.cpp
    src/cards/green/flowershop.cpp
    src/cards/green/fruitandvegetablemarket.cpp
    src/cards/green/furniturefactory.cpp
    src/cards/green/generalstore.cpp
    src/cards/green/sodabottlingplant.cpp

    src/cards/purple/publisher.cpp
    src/cards/purple/stadium.cpp
    src/cards/purple/taxoffice.cpp
    src/cards/purple/techstartup.cpp

    src/commands/gaincoinscommand.cpp
    # 如果有其他 .cpp 文件，请在此处添加
)

# 显式收集所有头文件。
# 包含头文件有助于 IDE 的代码分析和导航，但通常不需要在 add_executable 中直接列出，
# 除非它们包含 Q_OBJECT 宏且没有对应的 .cpp 文件（这种情况较少见）。
set(PROJECT_HEADERS
    mainwindow.h
    src/card.h
    src/cardfactory.h
    src/cardstore.h
    src/commandfactory.h
    src/dice.h
    src/gamecommand.h
    src/gamecontroller.h
    src/gamestate.h
    src/global.h # 纯头文件，通常没有对应的 .cpp
    src/player.h

    src/cards/blue/appleorchard.h
    src/cards/blue/flowerorchard.h
    src/cards/blue/forest.h
    src/cards/blue/mackerelboat.h
    src/cards/blue/ranch.h
    src/cards/blue/wheatfield.h

    src/cards/red/cafe.h
    src/cards/red/familyrestaurant.h
    src/cards/red/frenchrestaurant.h
    src/cards/red/membersonlyclub.h
    src/cards/red/pizzajoint.h
    src/cards/red/sushibar.h

    src/cards/green/bakery.h
    src/cards/green/cheesefactory.h
    src/cards/green/flowershop.h
    src/cards/green/fruitandvegetablemarket.h
    src/cards/green/furniturefactory.h
    src/cards/green/generalstore.h
    src/cards/green/sodabottlingplant.h

    src/cards/purple/publisher.h
    src/cards/purple/stadium.h
    src/cards/purple/taxoffice.h
    src/cards/purple/techstartup.h

    src/commands/gaincoinscommand.h
    # 如果有其他 .h 文件，请在此处添加
)

# 添加可执行目标。
# 将所有收集到的源文件、UI 文件和资源文件添加到构建中。
add_executable(${CMAKE_PROJECT_NAME}
    ${PROJECT_SOURCES}
    # 显式列出头文件在这里是可选的，但对于某些IDE集成有帮助
    ${PROJECT_HEADERS}
    mainwindow.ui
    src/randomutils.h src/randomutils.cpp
    src/commands/startturncommand.h src/commands/startturncommand.cpp
    src/commands/rolldicecommand.h src/commands/rolldicecommand.cpp
    src/commands/stealcoinscommand.h src/commands/stealcoinscommand.cpp

    # Machi_Koro_AI.qrc
)

# 设置项目的包含目录。
# ${CMAKE_SOURCE_DIR} 确保可以找到项目根目录下的头文件 (如 mainwindow.h)。
# ${CMAKE_SOURCE_DIR}/src 确保 src 目录下的头文件可以被直接包含 (如 #include "gamecontroller.h")。
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    # 如果 src/cards/blue 和 src/commands 下有需要直接包含的头文件，
    # 也可以添加这些目录，例如：
    # ${CMAKE_SOURCE_DIR}/src/cards/blue
    # ${CMAKE_SOURCE_DIR}/src/commands
)

# 链接 Qt 库到可执行目标。
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
)

# 处理翻译文件 (.ts)。
# 如果存在 Machi_Koro_AI_zh_CN.ts 文件，则使用 Qt6 的 qt6_add_translations 函数将其编译为 .qm 文件。
# 这些 .qm 文件通常在运行时通过 QTranslator 加载。
if (EXISTS "${CMAKE_SOURCE_DIR}/Machi_Koro_AI_zh_CN.ts")
    qt6_add_translations(${CMAKE_PROJECT_NAME}
        TS_FILES
        Machi_Koro_AI_zh_CN.ts
    )
    # 编译后的 .qm 文件会放置在构建目录中。
    # 在你的 C++ 代码中 (通常在 main.cpp)，你需要使用 QTranslator 来加载这些 .qm 文件。
    # 例如：
    # QTranslator translator;
    # if (translator.load("Machi_Koro_AI_zh_CN.qm", QApplication::applicationDirPath())) {
    #     a.installTranslator(&translator);
    # }
endif()

# 可选：添加一个自定义目标，用于更新 .ts 翻译文件。
# 这会运行 lupdate 工具，扫描源代码中的 tr() 调用，并更新 .ts 文件。
# 开发者可以运行 `cmake --build . --target update_translations` 来更新翻译。
add_custom_target(update_translations
    COMMAND ${Qt6_LUPDATE_EXECUTABLE} ${CMAKE_SOURCE_DIR} -ts Machi_Koro_AI_zh_CN.ts
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Updating translation files (.ts) using lupdate"
)

# 可选：定义安装规则。
# 这允许用户通过 `cmake --install .` 命令将编译好的应用程序安装到指定位置。
install(TARGETS ${CMAKE_PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # 对于 Windows/Linux 可执行文件
    BUNDLE DESTINATION ${CMAKE_INSTALL_BINDIR} # 对于 macOS 应用包
)
