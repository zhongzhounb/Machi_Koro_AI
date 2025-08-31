#include "activatecardscommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "dice.h"
#include "commandfactory.h"


ActivateCardsCommand::ActivateCardsCommand(Player* sourcePlayer, QObject* parent,QList<Card*> cards,Player* activePlayer)
    : GameCommand(CommandType::ActivateCards, sourcePlayer,parent,cards,activePlayer){
}

void ActivateCardsCommand::execute(GameState* state, GameController* controller){
    //获取当前骰子数
    Dice* dice=state->getDice();
    int rollSum=dice->getSum();

    //如果点数相同触发游乐园
    QList<Card*> cards=m_sourcePlayer->getCardsForName("游乐园");
    if(cards.last()->getState()==State::Opening&&dice->getFirstNum()==dice->getSecondNum())
        controller->addCommand(CommandFactory::instance().createCommand(cards.last()->getSpecialType(),m_sourcePlayer,controller,cards,m_sourcePlayer));

    //激活所有玩家的非地标建筑
    for(Player* player:state->getPlayers())
        for(QList<Card*> cards:player->getCards()){
            Card* card=cards.last();
            if(card->getType()!=Type::Landmark&&card->isActivate(player,m_sourcePlayer,rollSum))
                controller->addCommand(CommandFactory::instance().createCommand(card->getActivateType(),player,controller,cards,m_sourcePlayer));
        }

    //日志
    QString log=QString("投掷结果为%1,开始执行卡牌效果：").arg(rollSum);
    state->addLog(log);
};



