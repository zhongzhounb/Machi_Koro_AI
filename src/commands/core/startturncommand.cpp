#include "startturncommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "commandfactory.h"


StartTurnCommand::StartTurnCommand(Player* sourcePlayer,  QObject* parent)
    : GameCommand(CommandType::StartTurn, sourcePlayer,parent){
}

void StartTurnCommand::execute(GameState* state, GameController* controller) {
    /*创建基本命令到队列*/
    //抛骰子
    controller->addCommand(CommandFactory::instance().createRollDiceCommand(state->getCurrentPlayer(),controller));
    //执行卡牌效果
    controller->addCommand(CommandFactory::instance().createActivateCardsCommand(state->getCurrentPlayer(),controller));
    //判断领低保
    controller->addCommand(CommandFactory::instance().createGainOneCoinIfNoCoinsCommand(state->getCurrentPlayer(),controller));
    //买卡
    controller->addCommand(CommandFactory::instance().createBuyCardCommand(state->getCurrentPlayer(),controller));

    /*创建卡牌队列*/
    if(m_sourcePlayer->getCardNum("广播塔",State::Opening)>0)
        controller->addCommand(m_sourcePlayer->getCardSpecialCommand("广播塔"));
    //需要发一个回合开始信号
}

QString StartTurnCommand::getLog()const {
    return QString("====玩家 %1 的回合====").arg(m_sourcePlayer->getName());
}
