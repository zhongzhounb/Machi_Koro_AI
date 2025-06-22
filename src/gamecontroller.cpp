#include "gamecontroller.h"
#include "gamecommand.h"
#include "gamestate.h"
#include "player.h"

GameController::GameController(QObject* parent):QObject(parent){

}

void GameController::processNextCommand() {
    // 1. 如果当前有命令正在等待用户输入，则直接返回，等待UI回传选择。
    if (m_currentCommand && m_currentCommand->requiresUserInput()) {
        qDebug() << "Waiting for user input for command ID:" << m_currentCommand->getId();
        return;
    }

    // 2. 如果当前没有正在处理的命令，则从队列中取出下一个。
    if (!m_commandsQueue.isEmpty()) {
        // 取出队列中的第一个命令，并将其设为当前处理的命令。
        m_currentCommand = m_commandsQueue.takeFirst();
        emit logMessage(QString("准备执行命令: %1 (ID: %2)").arg(static_cast<int>(m_currentCommand->getType())).arg(m_currentCommand->getId()));

        // 3. 判断命令是否需要用户输入
        if (m_currentCommand->requiresUserInput()) {
            // 获取当前玩家
            Player* currentPlayer = m_state->getCurrentPlayer();
            if (!currentPlayer) {
                qWarning() << "Error: No current player set in GameState. Cannot process command requiring input.";
                // 错误处理：可能标记命令失败并结束
                onCommandFinished(m_currentCommand);
                return;
            }

            // 如果是真人玩家 (假设 AIRank::None 表示真人)
            if (currentPlayer->getAIRank() == AIRank::None) {
                // 发送提示数据给UI，等待UI通过 handleUserChoice 回传结果
                emit requestUserInputPrompt(m_currentCommand->getPromptData());
                // 命令现在处于等待状态，processNextCommand 退出，等待UI响应
            }
            // 如果是AI玩家
            else {
                // 获取命令的提示数据
                QVariantMap promptData = m_currentCommand->getPromptData();
                // 调用命令自身的 getAutoChoice 方法来获取AI的选择
                // 确保 getAutoChoice 是 const 方法，并且 GameState* state 也是 const
                QVariantMap aiChoice = m_currentCommand->getAutoChoice(promptData, m_state);
                m_currentCommand->setChoice(aiChoice); // 设置AI的选择

                // AI选择完成后，立即执行命令
                m_currentCommand->execute(m_state, this);
                // 命令执行完毕，通知控制器进行清理并处理下一个命令
                onCommandFinished(m_currentCommand);
            }
        } else {
            // 4. 命令不需要用户输入，直接执行
            m_currentCommand->execute(m_state, this);
            // 命令执行完毕，通知控制器进行清理并处理下一个命令
            onCommandFinished(m_currentCommand);
        }
    } else {
        qDebug() << "所有命令处理完毕，进入下一个游戏阶段或玩家回合。";
        // 游戏回合结束或进入下一阶段的逻辑
        // 例如：m_state->nextTurn();
        // 然后可能添加下一个玩家的 RollDiceCommand
    }
}

// 槽函数：接收UI回传的用户选择
void GameController::handleUserChoice(int commandId, const QVariantMap& choice) {
    qDebug() << "收到用户选择: Command ID =" << commandId << ", Choice =" << choice;

    // 确保当前命令存在、ID匹配且正在等待用户输入
    if (m_currentCommand && m_currentCommand->getId() == commandId && m_currentCommand->requiresUserInput()) {
        m_currentCommand->setChoice(choice); // 设置用户选择
        m_currentCommand->execute(m_state, this); // 执行命令
        onCommandFinished(m_currentCommand); // 命令执行完毕，通知控制器
    } else {
        qWarning() << "收到无效的用户选择或命令ID不匹配。Command ID:" << commandId;
    }
}

// 槽函数：命令执行完毕后调用，用于清理和推进队列
void GameController::onCommandFinished(GameCommand* command) {
    if (m_currentCommand == command) {
        emit logMessage(m_currentCommand->getLog()); // 记录日志
        m_currentCommand->deleteLater(); // 安全地删除命令对象
        m_currentCommand = nullptr; // 清空当前命令指针
        processNextCommand(); // 递归调用，处理队列中的下一个命令
    } else {
        qWarning() << "onCommandFinished: Finished command is not the current command. ID:" << command->getId();
        // 这通常不应该发生，但如果发生，仍然要清理传入的命令
        command->deleteLater();
    }
}
void GameController::addCommand(GameCommand* command){
    m_commandsQueue.push_back(command);
}

void GameController::delCommand(GameCommand* command){
    m_commandsQueue.removeOne(command);
}

QList<GameCommand*> GameController::getCommands(CommandType type){
    QList<GameCommand*> commands;
    for(GameCommand* command:m_commandsQueue)
        if(command->getType()==type)
            commands.append(command);
    return commands;
}
