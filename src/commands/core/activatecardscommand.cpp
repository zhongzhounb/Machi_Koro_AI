#include "activatecardscommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "dice.h"


ActivateCardsCommand::ActivateCardsCommand(Player* sourcePlayer,  QObject* parent)
    : GameCommand(CommandType::RollDice, sourcePlayer,parent){
}


void ActivateCardsCommand::execute(GameState* state, GameController* controller){
    //获取当前骰子数
    Dice* dice=state->getDice();
    m_rollSum=dice->getSum();

    //如果点数相同触发游乐园
    if(dice->getFirstNum()==dice->getSecondNum()&&m_sourcePlayer->getCardNum("游乐园",State::Opening))
        controller->addCommand(m_sourcePlayer->getCardSpecialCommand("游乐园"));

    //激活所有玩家的非地标建筑
    for(Player* player:state->getPlayers())
        for(QList<Card*> cards:player->getCards())
            if(cards.first()->getType()!=Type::Landmark&&cards.first()->isActivate(player,m_sourcePlayer,m_rollSum)){
                QList<GameCommand*> commands=cards.first()->createCommands(player,m_sourcePlayer);
                for(GameCommand* command:commands)
                    controller->addCommand(command);
            }
};

QString ActivateCardsCommand::getLog() const {
    return QString("投掷结果为%1,开始执行卡牌效果：").arg(m_rollSum);
};

