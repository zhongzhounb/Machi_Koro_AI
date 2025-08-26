#include "startturncommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "commandfactory.h"
#include "dice.h"

StartTurnCommand::StartTurnCommand(Player* sourcePlayer, QObject* parent,QList<Card*> cards,Player* activePlayer)
    : GameCommand(CommandType::StartTurn, sourcePlayer,parent,cards,activePlayer){
}

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
    QString log=QString("======玩家 %1 的回合======").arg(m_sourcePlayer->getName());
    state->addLog(log);
}
