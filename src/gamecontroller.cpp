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

    // 如果现在没有执行的命令，取一个命令
    if(m_currentCommand==nullptr){
        //如果没有命令，则说明当前玩家执行完成，则调用下一个玩家
        if(m_commandsQueue.empty()){
            //下一个玩家
            m_state->nextPlayer();
            //放入第一个指令
            addCommand(CommandFactory::instance().createCommand(StartTurn,m_state->getCurrentPlayer(),this));
        }
        m_currentCommand=m_commandsQueue.takeFirst();
    }

    qDebug()<<"正在执行："<<m_currentCommand->metaObject()->className();

    //判断是否要用户输入
    PromptData pd=m_currentCommand->getPromptData(m_state);

    //判断玩家是否为AI
    if(m_currentCommand->getSourcePlayer()&&m_currentCommand->getSourcePlayer()->getAIRank()==AIRank::None)
        pd.isAutoInput=false;
    emit requestUserInput(pd);

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
    qDebug() << "收到用户选择: Choice =" << optionId;

    if (m_currentCommand) {
        bool isFinish=m_currentCommand->setInput(optionId,m_state,this); // 设置用户选择
        if(isFinish)
            onCommandFinished(m_currentCommand); // 命令执行完毕，通知控制器
        else
            processNextCommand();

    } else {
        qWarning() << "收到无效的用户选择" ;
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

