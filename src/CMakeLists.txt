# 定义核心游戏逻辑库
# 这里定义为 STATIC 库，也可以是 SHARED
add_library(Machi_Koro_AI_Core STATIC
    gamecontroller.h
    gamecontroller.cpp
    gamestate.h
    gamestate.cpp
    player.h
    player.cpp
    card.h
    card.cpp
    cardfactory.h
    cardfactory.cpp
    gamecommand.h
    gamecommand.cpp
    commandfactory.h
    commandfactory.cpp
    dice.h
    dice.cpp
    randomutils.h # 如果 randomutils.h 是纯头文件，它不需要对应的 .cpp 文件
)

# 收集 AI 相关的源文件
list(APPEND Machi_Koro_AI_Core_SOURCES
    ai/aibase.h
    ai/aibase.cpp
    ai/simpleai.h
    ai/simpleai.cpp
)

# 使用 GLOB_RECURSE 收集 cards/ 目录下的所有 .h 和 .cpp 文件
# 警告：GLOB_RECURSE 在添加新文件时可能不会自动触发 CMake 重新配置
file(GLOB_RECURSE CARD_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/cards/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/cards/*.cpp"
)
list(APPEND Machi_Koro_AI_Core_SOURCES ${CARD_SOURCES})

# 使用 GLOB_RECURSE 收集 commands/ 目录下的所有 .h 和 .cpp 文件
# 警告：GLOB_RECURSE 在添加新文件时可能不会自动触发 CMake 重新配置
file(GLOB_RECURSE COMMAND_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/commands/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/commands/*.cpp"
)
list(APPEND Machi_Koro_AI_Core_SOURCES ${COMMAND_SOURCES})

# 将所有收集到的源文件添加到库中
target_sources(Machi_Koro_AI_Core PRIVATE ${Machi_Koro_AI_Core_SOURCES})

# 设置库的公共和私有头文件搜索路径
# PUBLIC 意味着使用此库的其他目标（如 Machi_Koro_AI_App）也将获得这些包含路径
target_include_directories(Machi_Koro_AI_Core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # 包含 src/ 自身
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ai>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cards>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/commands>
)

# 链接库可能需要的 Qt 模块
# 假设核心逻辑可能使用 Qt 的基本类型（如 QString, QList, QObject）
target_link_libraries(Machi_Koro_AI_Core
    Qt6::Core
)