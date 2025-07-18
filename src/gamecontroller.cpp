#include "gamecontroller.h"
#include "gamecommand.h"
#include "gamestate.h"
#include "player.h"
#include "commandfactory.h"
#include "logviewerwidget.h"
#include "cardstore.h"
#include "cardstoreareawidget.h"


GameController::GameController(MainWindow* mainWindow,GameState* state,QObject* parent):m_mainWindow(mainWindow),m_state(state),m_currentCommand(nullptr),QObject(parent){

}

void GameController::initializeGame(){
    //设置信号与槽连接
    setupConnections();

    //放入第一个指令
    addCommand(CommandFactory::instance().createInitGameCommand(this));
    //执行
    processNextCommand();

}

void GameController::setupConnections(){



    /*CardStoreAreaWidget* cardStoreAreaWidget=m_mainWindow->getCardStoreAreaWidget();
    cardStoreAreaWidget->setGameState(m_state);*/

}

void GameController::processNextCommand() {
    // 如果当前有命令正在等待用户输入，则直接返回，等待UI回传选择。
    if (m_currentCommand && m_currentCommand->getPromptData(m_state).type==PromptData::PromptType::None) {
        qDebug() << "Waiting for user input for command ID:" << m_currentCommand->getId();
        return;
    }

    // 如果现在没有执行的命令，取一个命令
    if(m_currentCommand==nullptr){
        //如果没有命令，则说明当前玩家执行完成，则调用下一个玩家
        if(m_commandsQueue.empty()){
            //下一个玩家
            m_state->nextPlayer();
            //放入第一个指令
            addCommand(CommandFactory::instance().createStartTurnCommand(m_state->getCurrentPlayer(),this));
        }
        m_currentCommand=m_commandsQueue.takeFirst();
    }


    qDebug()<<1;
    //判断是否要用户输入
    if(m_currentCommand->getPromptData(m_state).type!=PromptData::PromptType::None){
        //判断玩家是否为AI
        qDebug()<<2;
        if(m_currentCommand->getSourcePlayer()->getAIRank()==AIRank::None){
            qDebug()<<3;
            //向前端发出信号
            return ;
        }
        qDebug()<<4;
        //如果是AI，只要返回选择就行（目前无考虑AI）
        //m_currentCommand->setChoice(m_currentCommand->getAutoChoice(m_currentCommand->getPromptData(),m_state));
    }
    qDebug()<<5;



    //执行
    m_currentCommand->execute(m_state,this);

    //清理命令并自动调用下一个命令
    onCommandFinished(m_currentCommand);

}

// 槽函数：接收UI回传的用户选择
void GameController::handleUserChoice(int commandId, const QVariantMap& choice) {
    qDebug() << "收到用户选择: Command ID =" << commandId << ", Choice =" << choice;

    // 确保当前命令存在、ID匹配且正在等待用户输入
    if (m_currentCommand && m_currentCommand->getId() == commandId && m_currentCommand->getPromptData(m_state).type==PromptData::PromptType::None) {
        //m_currentCommand->setChoice(choice); // 设置用户选择
        m_currentCommand->execute(m_state, this); // 执行命令
        onCommandFinished(m_currentCommand); // 命令执行完毕，通知控制器
    } else {
        qWarning() << "收到无效的用户选择或命令ID不匹配。Command ID:" << commandId;
    }
}

// 槽函数：命令执行完毕后调用，用于清理和推进队列
void GameController::onCommandFinished(GameCommand* command) {
    if (m_currentCommand == command) {
        m_state->addLog(m_currentCommand->getLog()); // 记录日志
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
