#include "airport.h"
#include "commandfactory.h"

Airport::Airport(QObject* parent):
    Card("机场", 30, Color::Landmark, Type::Landmark, 10 , 0, 0, parent,State::Closing) {}

QString Airport::getDescription() const {
    return QString("若你在本回合没有建造任何建筑，你获得 %1 金币。").arg(m_value);
}

double Airport::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

GameCommand* Airport::createSpecialCommand(Player* owner){
    return CommandFactory::instance().createGainCoinsIfNoBuyCardCommand(owner,this,this);
}

//地标卡无激活命令
QList<GameCommand*> Airport::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {};
}
