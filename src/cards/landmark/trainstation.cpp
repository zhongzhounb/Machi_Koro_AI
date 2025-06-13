#include "trainstation.h"
#include "player.h"

TrainStation::TrainStation(QObject* parent):
    Card("火车站", 4, Color::Landmark, Type::Landmark, -1, 0, 0, "你的回合，可以选择抛一个或两个骰子",parent) {}

double TrainStation::getBuyWight(Player* aiPlayer, Game* game) const {
    return 0.0;
}

/*火车站效果提示*/
//火车站激活效果体现在抛骰子效果上，所以激活后无效果
QString TrainStation::activate(Player* owner, Player* activePlayer, Game* game, const QVariant& choiceData){
    Q_UNUSED(owner);Q_UNUSED(activePlayer);Q_UNUSED(game);
    return "";
}

bool Card::isActivate(Player* owner, Player* activePlayer,int rollSum) const {
    return this->getState()==State::Opening;
}
