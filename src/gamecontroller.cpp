#include "gamecontroller.h"
#include "gamecommand.h"
#include "gamestate.h"
#include "player.h"
#include "commandfactory.h"
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

    //判断是否为新的命令
    bool isNewCommand=false;

    //当前没有正执行的命令，就取队首命令
    if (m_currentCommand == nullptr) {
        isNewCommand=true;

        //空队列，表示当前玩家回合完成，切换玩家并开始新的回合
        if (m_commandsQueue.empty()) {
            m_state->nextPlayer();
            addCommand(CommandFactory::instance().createCommand(StartTurn, m_state->getCurrentPlayer(), this));
        }
        m_currentCommand = m_commandsQueue.takeFirst();

        //并行队列检查器：检测队列中是否有连续可执行卡牌队列
        m_parallelQueue.append(m_currentCommand);
        while (!m_commandsQueue.isEmpty()) {
            GameCommand* nextCmd = m_commandsQueue.first();
            if (nextCmd->getCard() &&
                m_currentCommand->getCard() &&
                nextCmd->getCard()->getNameId() == m_currentCommand->getCard()->getNameId()) {
                m_parallelQueue.append(nextCmd);
                m_commandsQueue.removeFirst();
            } else
                break;
        }
    }

    // 并行队列执行：可执行一条或多条
    int delay = 0;
    for (auto* cmd : m_parallelQueue) {
        QTimer::singleShot(delay, this, [this, cmd,isNewCommand]() {
            //如果是新命令，并且是卡牌命令，则先执行卡牌动画
            if(isNewCommand&&cmd->getCard()){
                //卡牌进入动画prompt
                PromptData pd;
                pd.type=PromptData::CardInAnimation;
                pd.cards=cmd->getCards();
                pd.buyer=cmd->getSourcePlayer();
                emit requestUserInput(pd);
                //不要回复
                m_needResponse=false;
            }
            else{
                //获取prompt
                PromptData pd = cmd->getPromptData(m_state);
                //设置自动返回
                pd.isAutoInput = cmd->getSourcePlayer() &&
                                 cmd->getSourcePlayer()->getAIRank() != AIRank::None;
                //发送
                emit requestUserInput(pd);
            }

        });
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
    //收到新回复
    m_responsesNum++;

    //如果收到了并发数量的信号，说明执行完毕了当前阶段(大于的情况用来处理最后的卡牌弹出动画
    if(m_responsesNum>=m_parallelQueue.size()){
        //如果不要回复，则直接下一个命令，并更新回复标志
        if(!m_needResponse)
            m_needResponse=true;
        else{
            //执行下一步并判断是否完成命令
            bool finished=true;
            for (GameCommand* cmd : m_parallelQueue){
                //如果是并行程序，一定是自动输入，所以optionId不重要，否则，串行程序最后一个即所有，一定输入optionId
                finished = cmd->setInput(optionId, m_state, this);
            }

            //完成后需要清理命令
            if(finished){
                //卡牌退出动画
                if(m_parallelQueue.first()->getCard()){
                    PromptData pd;
                    pd.type=PromptData::CardOutAnimation;
                    emit requestUserInput(pd);
                    m_needResponse=false;
                    //删除命令
                    for (GameCommand* cmd : m_parallelQueue)
                        onCommandFinished(cmd);
                    m_parallelQueue.clear();
                    return;
                }

                //删除命令
                for (GameCommand* cmd : m_parallelQueue)
                    onCommandFinished(cmd);
                m_parallelQueue.clear();
            }
        }

        processNextCommand();
        m_responsesNum=0;
    }
}

// 槽函数：命令执行完毕后调用，用于清理（不推进队列！）
void GameController::onCommandFinished(GameCommand* command) {
    //判定是否有人结束
    if(m_state->getCurrentPlayer()&&checkWin()){
        m_state->addLog(QString("游戏结束！玩家 %1 获胜！").arg(m_state->getCurrentPlayer()->getName()));
        return;
    }
    if (m_currentCommand == command) {
        m_currentCommand->deleteLater(); // 安全地删除命令对象
        m_currentCommand = nullptr; // 清空当前命令指针
    } else {
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

