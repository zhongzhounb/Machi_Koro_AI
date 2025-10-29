#include "gamecontroller.h"
#include "gamecommand.h"
#include "gamestate.h"
#include "player.h"
#include "commandfactory.h"
#include "logviewerwidget.h"
#include "cardstore.h"
#include "cards/cardstoreareawidget.h"
#include <QTimer.h>
#include "card.h"
#include "parallelcommandgroup.h"

GameController::GameController(MainWindow* mainWindow,GameState* state,QObject* parent):m_mainWindow(mainWindow),m_state(state),m_currentCommand(nullptr),QObject(parent){

}

void GameController::initializeGame(){
    //设置信号与槽连接
    setupConnections();

    GameCommand* command=CommandFactory::instance().createCommand(InitGame,nullptr,this);
    qDebug()<<"创建command完成";
    //放入第一个指令
    addCommand(command);

    //执行
    processNextCommand();
}

void GameController::setupConnections(){

}

void GameController::processNextCommand() {
    if (m_currentCommand == nullptr) {
        if (m_commandsQueue.empty()) {
            m_state->nextPlayer();
            addCommand(CommandFactory::instance().createCommand(StartTurn, m_state->getCurrentPlayer(), this));
        }

        m_currentCommand = m_commandsQueue.takeFirst();

        // --- 新增：检查是否可以组成并行批次 ---
        m_parallelQueue.clear();
        m_parallelQueue.append(m_currentCommand);

        while (!m_commandsQueue.isEmpty()) {
            GameCommand* nextCmd = m_commandsQueue.first();
            if (nextCmd->getCard() &&
                m_currentCommand->getCard() &&
                nextCmd->getCard()->getNameId() == m_currentCommand->getCard()->getNameId()) {
                m_parallelQueue.append(nextCmd);
                m_commandsQueue.removeFirst();
            } else {
                break;
            }
        }
    }

    // --- 如果并行队列中有多个命令 ---
    if (m_parallelQueue.size() > 1) {
        qDebug() << "检测到并行命令批次 size=" << m_parallelQueue.size();
        m_pendingResponses.clear();
        for (auto* cmd : m_parallelQueue)
            m_pendingResponses.insert(cmd->getId());

        // 间隔300ms依次发出requestUserInput信号
        int delay = 0;
        for (auto* cmd : m_parallelQueue) {
            QTimer::singleShot(delay, this, [this, cmd]() {
                PromptData pd = cmd->getPromptData(m_state);
                pd.isAutoInput = cmd->getSourcePlayer() &&
                                 cmd->getSourcePlayer()->getAIRank() != AIRank::None;
                emit requestUserInput(pd);
            });
            delay += 300;
        }
    } else {
        // 原逻辑保持不变
        PromptData pd = m_currentCommand->getPromptData(m_state);
        pd.isAutoInput = m_currentCommand->getSourcePlayer() &&
                         m_currentCommand->getSourcePlayer()->getAIRank() != AIRank::None;
        emit requestUserInput(pd);
    }
}



bool GameController::checkWin(){
    for(QList<Card*> cards:m_state->getCurrentPlayer()->getCards()){
        Card* card=cards.last();
        if(card->getType()==Type::Landmark&&card->getState()==State::Closing)
            return false;
    }
    return true;
}

// 槽函数：接收UI回传的用户选择
void GameController::onResponseUserInput(int optionId) {
    qDebug() << "收到用户选择: " << optionId;

    if (m_parallelQueue.size() > 1) {
        // --- 并行模式 ---
        if (m_pendingResponses.isEmpty()) {
            qWarning() << "onResponseUserInput called but no pending responses.";
            return;
        }

        // 移除一个响应
        int finishedId = *m_pendingResponses.begin(); // 顺序固定A->B->C
        m_pendingResponses.remove(finishedId);

        // 如果全部响应收到
        if (m_pendingResponses.isEmpty()) {
            bool allFinished = true;

            // 执行每个命令的setInput逻辑
            for (GameCommand* cmd : m_parallelQueue) {
                bool finished = cmd->setInput(optionId, m_state, this);
                if (!finished)
                    allFinished = false;
            }

            // 如果还有后续步骤（false）
            if (!allFinished) {
                int delay = 0;
                for (auto* cmd : m_parallelQueue) {
                    QTimer::singleShot(delay, this, [this, cmd]() {
                        PromptData pd = cmd->getPromptData(m_state);
                        pd.isAutoInput = cmd->getSourcePlayer() &&
                                         cmd->getSourcePlayer()->getAIRank() == AIRank::None;
                        emit requestUserInput(pd);
                    });
                    delay += 300;
                }
            } else {
                // 所有命令都结束
                for (auto* cmd : m_parallelQueue)
                    cmd->deleteLater();
                m_parallelQueue.clear();
                m_currentCommand = nullptr;
                processNextCommand();
            }
        }
    } else {
        // --- 单命令模式 ---
        if (m_currentCommand) {
            bool isFinish = m_currentCommand->setInput(optionId, m_state, this);
            if (isFinish)
                onCommandFinished(m_currentCommand);
            else
                processNextCommand();
        }
    }
}

// 槽函数：命令执行完毕后调用，用于清理和推进队列
void GameController::onCommandFinished(GameCommand* command) {
    //判定是否有人结束
    if(m_state->getCurrentPlayer()&&checkWin()){
        m_state->addLog(QString("游戏结束！玩家 %1 获胜！").arg(m_state->getCurrentPlayer()->getName()));
        return;
    }
    if (m_currentCommand == command) {
        m_currentCommand->deleteLater(); // 安全地删除命令对象
        m_currentCommand = nullptr; // 清空当前命令指针
        processNextCommand(); // 递归调用，处理队列中的下一个命令
    } else {// 这通常不应该发生，但如果发生，仍然要清理传入的命令
        qWarning() << "onCommandFinished: Finished command is not the current command. ID:" << command->getId();
        command->deleteLater();
    }
}
void GameController::addCommand(GameCommand* command){
    int newPriority = command->getPriority();
    bool inserted = false;
    for (int i = 0; i < m_commandsQueue.size(); ++i) {
        // 找到第一个优先级大于或等于新命令优先级的位置
        if (m_commandsQueue[i]->getPriority() >= newPriority) {
            m_commandsQueue.insert(i, command);
            inserted = true;
            break;
        }
    }
    // 如果新命令的优先级是最高的（或队列为空），则添加到末尾
    if (!inserted) {
        m_commandsQueue.push_back(command);
    }

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

