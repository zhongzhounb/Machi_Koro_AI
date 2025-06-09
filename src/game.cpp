// Game.cpp (命令调度部分)

// 在 Game::startGame() 中
void Game::startGame(int numPlayers) {
    // ... (初始化玩家等) ...
    startTurn();
}

// 在 Game::startTurn() 中
void Game::startTurn() {
    m_currentPlayer = m_players[m_currentPlayerIndex];
    m_commandQueue.clear(); // 清空上一回合的命令

    m_game->logEvent(QString("现在是 %1 的回合。").arg(m_currentPlayer->getName()));
    m_playerRollModifiers.clear(); // 清除上回合的骰子修正
    m_isRerolling = false; // 重置重掷标记

    // 火车站逻辑：在掷骰子前询问
    if (playerHasLandmark(m_currentPlayer, 101)) { // 101是火车站ID
        // 添加一个命令，让玩家选择掷骰子数量
        // 这个命令需要玩家交互，所以会暂停队列
        m_commandQueue.append(new ChooseDiceCountCommand(m_currentPlayer.data(), this));
    } else {
        // 没有火车站，直接添加掷骰子命令（默认1个骰子，AI会覆盖）
        m_commandQueue.append(new RollDiceCommand(m_currentPlayer.data(), this, 1));
    }

    // 添加处理卡牌效果的命令 (这个命令内部会处理所有掷骰子激活的卡牌效果)
    m_commandQueue.append(new ProcessCardEffectsCommand(m_currentPlayer.data(), this, 0, 0)); // 骰子点数和数量在RollDiceCommand中设置

    // 添加购买卡牌命令
    m_commandQueue.append(new BuyCardCommand(m_currentPlayer.data(), this));

    // 添加处理回合结束前选择的命令 (科技公司)
    m_commandQueue.append(new ProcessEndOfTurnChoicesCommand(m_currentPlayer.data(), this));

    // 添加结束回合命令
    m_commandQueue.append(new EndTurnCommand(m_currentPlayer.data(), this));

    processNextCommand(); // 开始执行队列
}

// 核心命令调度函数
void Game::processNextCommand() {
    if (m_currentCommand && m_currentCommand->getStatus() == GameCommand::Executing) {
        // 如果当前命令还在执行中（例如等待玩家选择），则不处理下一个命令
        return;
    }

    if (!m_commandQueue.isEmpty()) {
        m_currentCommand = m_commandQueue.takeFirst();
        if (m_currentCommand->execute()) {
            // 如果命令立即完成，则继续处理下一个命令
            processNextCommand();
        }
        // 如果命令没有立即完成 (execute 返回 false)，它会发出请求UI交互的信号，
        // 游戏会暂停，等待UI/AI通过 onPlayerChoiceMade 等槽函数回调。
    } else {
        // 队列为空，回合结束，切换到下一个玩家的回合
        // 这部分逻辑将由 EndTurnCommand 处理
        // 如果 EndTurnCommand 已经执行完毕，这里就不需要再做任何事情了
    }
}

// 接收UI/AI的反馈，并通知当前命令继续
void Game::onPlayerChoiceMade(Player* chooser, const QVariant& choiceData) {
    if (m_currentCommand && m_currentCommand->getStatus() == GameCommand::Executing) {
        m_currentCommand->processChoice(choiceData);
        processNextCommand(); // 选择处理完毕，继续执行队列
    } else {
        qWarning() << "Received unexpected player choice (command not executing).";
    }
}

void Game::onPlayerChoiceSkipped(Player* chooser) {
    if (m_currentCommand && m_currentCommand->getStatus() == GameCommand::Executing) {
        m_currentCommand->processChoice(QVariant()); // 传递空QVariant表示跳过
        processNextCommand();
    } else {
        qWarning() << "Received unexpected player choice skipped (command not executing).";
    }
}

// 接收掷骰子数量选择 (火车站)
void Game::onDiceRollCountChosen(Player* chooser, int numDice) {
    if (m_currentCommand && m_currentCommand->getStatus() == GameCommand::Executing) {
        // 假设 ChooseDiceCountCommand 会处理这个回调
        m_currentCommand->processChoice(QVariant(numDice)); // 传递选择的骰子数量
        processNextCommand();
    } else {
        qWarning() << "Received unexpected dice roll count choice.";
    }
}

// 广播塔重掷请求
void Game::onRerollRequested(Player* rerollingPlayer) {
    if (rerollingPlayer == m_currentPlayer) {
        m_game->logEvent(QString("%1 正在重掷骰子...").arg(rerollingPlayer->getName()));
        // 清空当前队列中剩余的命令，并重新开始当前玩家的回合
        m_commandQueue.clear();
        m_isRerolling = true; // 标记正在重掷，避免广播塔再次触发
        startTurn(); // 重新开始当前玩家的回合，会生成新的命令序列
    } else {
        qWarning() << "Attempted to request reroll for non-current player.";
    }
}

// ... 其他辅助函数和卡牌逻辑 (如 getModifiedIncome, playerHasLandmark 等) ...
// 这些辅助函数现在由 GameCommand 内部调用，而不是 Game 自身的主流程。
