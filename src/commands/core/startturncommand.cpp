#include "startturncommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "commandfactory.h"
#include "dice.h"

StartTurnCommand::StartTurnCommand(Player* sourcePlayer, QObject* parent,QList<Card*> cards,Player* activePlayer)
    : GameCommand(CommandType::StartTurn, sourcePlayer,parent,cards,activePlayer){
    qDebug()<<"StartTurnCommand已创建";
}

//取期望收益最大的骰子数投掷
PromptData StartTurnCommand::getPromptData(GameState* state) const{
    PromptData pt;
    switch (m_currentStep){
    case 1:{//选择骰子个数阶段
        pt.type=PromptData::StartTurnAnimation;
        //前端会自己设置间隔“加空格”
        pt.promptMessage=QString("%1的回合").arg(m_sourcePlayer->getName());
        return pt;
    }
    }
    return pt;
}

// 设置选项，返回是否要继续获得选项（无选项时禁止调用）
bool StartTurnCommand::setInput(int optionId,GameState* state,GameController* controller) {
    switch (m_currentStep){
    case 1:{
        execute(state,controller);
        return true;
    }
    }
    return true;
};

void StartTurnCommand::execute(GameState* state, GameController* controller) {
    /*创建基本命令到队列*/
    //抛骰子
    controller->addCommand(CommandFactory::instance().createCommand(RollDice,m_sourcePlayer,controller));

    //执行卡牌效果
    controller->addCommand(CommandFactory::instance().createCommand(ActivateCards,m_sourcePlayer,controller));

    //判断领低保
    controller->addCommand(CommandFactory::instance().createCommand(GainOneCoinIfNoCoins,m_sourcePlayer,controller));

    //买卡
    controller->addCommand(CommandFactory::instance().createCommand(BuyCard,m_sourcePlayer,controller));

    /*创建卡牌队列*/
    addCommand("广播塔",controller);

    //清空骰子数
    state->getDice()->clearDice();
    state->getDiceTemp()->clearDice();
    QString log=QString("======玩家 %1 的回合======").arg(m_sourcePlayer->getName());
    state->addLog(log);
}
