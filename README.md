# Machi Koro Qt (骰子街 Qt 版) 🎲

## 简介

本项目是基于 C++ 和 Qt 框架实现的桌面版《骰子街》(Machi Koro) 游戏。它旨在提供一个功能完善、交互流畅的单机游戏体验，并特别注重采用现代软件设计模式，如**命令模式**和**前后端分离**的思想，以确保代码的清晰性、可维护性和可扩展性。

如果你是 Qt/C++ 开发者，对桌面游戏开发感兴趣，或者仅仅是《骰子街》这款经典桌游的爱好者，那么这个项目值得你探索！

**什么是《骰子街》？**
《骰子街》是一款快节奏的骰子和卡牌游戏，玩家通过购买和激活各种建筑来赚取金币，目标是第一个建造完所有地标建筑。游戏融合了运气（骰子）和策略（建筑组合）。

## ✨ 核心特性

*   **完整游戏流程：** 支持《骰子街》基础版的核心游戏规则，包括：
    *   抛掷一个或两个骰子（需开通【火车站】）。
    *   根据骰子点数激活建筑并获得收益。
    *   购买商店中的新建筑卡牌。
    *   建造地标建筑。
*   **多种玩家类型：**
    *   **真人玩家：** 通过交互式UI进行操作。
    *   **AI 玩家：** 实现基础的AI决策逻辑，可作为对手。
*   **命令模式驱动：** 游戏中的所有操作（抛骰子、购买卡牌、激活技能等）均封装为命令，易于扩展和管理。
*   **前后端分离架构：** 逻辑核心与UI展示解耦，提升了系统的灵活性和可测试性。
*   **游戏日志：** 实时记录游戏进程，便于回顾和调试。
*   **前置动画支持：** 在执行特定命令前，UI可播放相应的前置动画，提升用户体验。

## 🚀 技术栈

*   **核心语言：** C++
*   **UI 框架：** Qt 6 (或兼容 Qt 5.x)
*   **构建系统：** CMake

## 📐 架构设计亮点

本项目在设计上采用了以下关键模式和思想，以构建一个健壮且可扩展的游戏系统：

### 1. 命令模式 (Command Pattern)

游戏中的每一个可执行动作（如抛骰子、购买建筑、激活卡牌技能）都被封装成一个继承自 `GameCommand` 的独立对象。这带来了以下优势：
*   **解耦：** 请求者（`GameController`）与执行者（具体命令）解耦。
*   **可扩展性：** 添加新的游戏动作只需创建新的 `GameCommand` 子类，无需修改核心逻辑。
*   **可撤销/重做 (未来扩展)：** 命令对象可以轻松支持撤销和重做功能（本项目暂未实现）。
*   **日志与回放：** 每个命令都能生成自身的日志信息，为游戏回放提供了基础。

### 2. 前后端分离与 `QVariantMap` 通信

*   **核心思想：** 游戏的核心逻辑（后端）与用户界面（前端）严格分离。后端不直接操作UI元素，UI也不直接包含游戏规则。
*   **通信协议：** 使用 `QVariantMap` 作为前后端之间传递信息的统一数据包。
    *   **后端 -> 前端：** 当命令需要用户输入或前置动画时，`GameController` 会发出信号，携带一个 `QVariantMap`。这个 `QVariantMap` 包含了UI渲染所需的所有信息（例如，提示文本、所有可选选项及其启用状态、动画类型和参数等）。UI层仅根据这些数据进行渲染，无需硬编码游戏规则。
    *   **前端 -> 后端：** 用户在UI上做出选择后，UI将选择结果封装成 `QVariantMap`，通过 `GameController` 的槽函数回传给后端。
*   **`GameCommand` 的关键接口：**
    *   `virtual QVariantMap getPromptData() const;`：命令提供给UI/AI的提示信息。
    *   `virtual QVariantMap getPreAnimationData() const;`：命令执行前需要播放的动画数据。
    *   `void setChoice(const QVariantMap& choice);`：接收UI/AI做出的选择。

### 3. AI 决策集成

*   **`virtual QVariantMap getAutoChoice(const QVariantMap& promptData, const GameState* state) const;`**
    *   AI玩家的决策逻辑被集成在 `GameCommand` 的 `getAutoChoice` 方法中。当轮到AI玩家执行需要输入的命令时，`GameController` 会调用此方法，传入命令的提示数据和当前游戏状态，由命令自身根据这些信息生成AI的选择。
    *   **设计考量：** 尽管在大型项目中AI逻辑常被分离到独立的AI模块中，本项目将其集成在命令中，以简化特定场景下的决策流程，并确保AI能直接访问命令所需的上下文信息。

### 4. 游戏状态管理

*   `GameState` 类负责维护整个游戏的所有数据，包括玩家信息、建筑、金币、回合数等。它是一个纯粹的数据容器，不包含游戏逻辑。

### 5. 骰子模块化

*   独立的 `Dice` 类负责骰子的随机数生成和结果通知，并通过信号发出骰子抛掷事件，实现了骰子功能的解耦。

## ⚙️ 如何运行

### 前提条件

*   安装 Qt SDK (推荐 Qt 6.x，或兼容的 Qt 5.x 版本)。
*   安装 CMake (版本 3.14 或更高)。
*   C++ 编译器 (如 GCC, Clang, MSVC)。

### 构建步骤

1.  **克隆仓库：**
    ```bash
    git clone https://github.com/YourGitHubUsername/MachiKoroQt.git
    cd MachiKoroQt
    ```

2.  **创建构建目录并配置 CMake：**
    ```bash
    mkdir build
    cd build
    cmake ..
    ```
    *   如果你有多个 Qt 版本，可能需要指定 Qt 的安装路径，例如：
        `cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64 ..`

3.  **编译项目：**
    ```bash
    cmake --build .
    ```
    或在 Linux/macOS 上使用 `make`：
    ```bash
    make
    ```
    在 Windows 上使用 Visual Studio 构建工具：
    ```bash
    msbuild YourProjectName.sln
    ```

### 运行应用

编译成功后，可执行文件通常会在 `build` 目录下的 `bin` 或 `Debug`/`Release` 子目录中。

```bash
# 例如 (Linux/macOS)
./MachiKoroQt

# 例如 (Windows)
.\Debug\MachiKoroQt.exe
```
