#include "amusementpark.h"
#include "commandfactory.h"

AmusementPark::AmusementPark(QObject* parent):
    Card("游乐园", 16, Color::Landmark, Type::Landmark, -1 , 0, 0, parent,State::Closing) {}

QString AmusementPark::getDescription() const {
    return QString("若你抛出的两个骰子点数相同，该回合结束后还是你的回合。").arg(m_value);
}

double AmusementPark::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

GameCommand* AmusementPark::createSpecialCommand(Player* owner){
    return CommandFactory::instance().createGainNewTurnCommand(owner,this,this);
}

//地标卡无激活命令
QList<GameCommand*> AmusementPark::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {};
}

