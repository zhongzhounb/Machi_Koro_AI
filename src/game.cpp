#include<game.h>
#include<player.h>
#include<dice.h>
// 在 Game::startGame() 中
void Game::startGame(QList<Player*> players) {
    // 初始化玩家
    Game::startTurn(players.takeFirst());
}

// 在 Game::startTurn() 中
void Game::startTurn(Player* player) {
    m_currentPlayer = player;

    // 清空上一回合的命令
    m_commandQueue.clear();

    // 清除上回合的骰子修正
    m_dice->clearDice();

    // 掷骰子命令（若没火车站，按钮：投掷，否则按钮：掷一个、投掷两个）
    m_commandQueue.append(new RollDiceCommand(player, this));

    //【广播塔】重抛命令（【弹窗】提示：结果为：x，是否重置，按钮同上，增加不重抛）
    //【游乐园】如果两个骰子且相等，提示：此点数可触发游乐园）
    m_commandQueue.append(new RollDiceCommand(player, this));

    //【港口】如果抛到>=10选择是否+2（【弹窗】提示：是否数字+2，按钮：+2，不加）
    m_commandQueue.append(new AddTowCommand(player, this));

    // 执行所有卡牌命令（含主动技）
    m_commandQueue.append(new CardsActivateCommand(player, this));

    // 购买卡牌命令（显示：所有可购买选项、跳过）（如果没钱则自动跳过）
    m_commandQueue.append(new BuyCardCommand(player,  this));

    // 【科技公司】显示是否存钱选项（【弹窗】显示：存一元、跳过）（如果没钱则自动跳过）
    m_commandQueue.append(new TechnologyCompanyCommand(player,this));

    // 添加结束回合命令
    m_commandQueue.append(new EndTurnCommand(player, this));

    // 开始执行队列
    processNextCommand();
}

// 核心命令调度函数
void Game::processNextCommand() {
    // 如果当前命令还在执行中（例如等待玩家选择），则不处理下一个命令
    if (m_currentCommand && m_currentCommand->getStatus() == GameCommand::Executing)
        return;

    if (!m_commandQueue.isEmpty()) {
        m_currentCommand = m_commandQueue.takeFirst();
        // 如果命令立即完成，则继续处理下一个命令
        if (m_currentCommand->execute())
            processNextCommand();
        // 如果命令没有立即完成 (execute 返回 false)，它会发出请求UI交互的信号，
        // 游戏会暂停，等待UI/AI通过 onPlayerChoiceMade 等槽函数回调。
    }

    //到这里游戏结束
}

// 接收UI/AI的反馈，并通知当前命令继续
void Game::onPlayerChoice(Player* chooser, const QVariant& choiceData) {
    if (m_currentCommand && m_currentCommand->getStatus() == GameCommand::Executing) {
        m_currentCommand->processChoice(choiceData);
        processNextCommand(); // 选择处理完毕，继续执行队列
    } else {
        qWarning() << "Received unexpected player choice (command not executing).";
    }
}

