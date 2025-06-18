#include "mackerelboat.h"
#include "commandfactory.h"
#include "player.h"

MackerelBoat::MackerelBoat(QObject* parent):
    Card("鲭鱼船", 2, Color::Blue, Type::Fishery,3, 8, 8, parent) {}

QString MackerelBoat::getDescription() const {
    return QString("若你建造了【港口】，获得 %1 金币。").arg(this->m_value);
}

double MackerelBoat::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> MackerelBoat::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    //如果没有建港口，激活失败
    if(owner->getCardNum("港口",State::Opening)==0)
        return {CommandFactory::instance().createGainCoinsCommand(owner,this,this,true,"没建【港口】，船无法停靠。")};

    return {CommandFactory::instance().createGainCoinsCommand(owner,this,this)};
}
