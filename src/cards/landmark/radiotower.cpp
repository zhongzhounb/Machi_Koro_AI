#include "radiotower.h"
#include "commandfactory.h"

RadioTower::RadioTower(QObject* parent):
    Card("游乐园", 16, Color::Landmark, Type::Landmark, -1 , 0, 0, parent,State::Closing) {}

QString RadioTower::getDescription() const {
    return QString("你可以选择重新抛一次骰子。").arg(m_value);
}

double RadioTower::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

GameCommand* RadioTower::createSpecialCommand(Player* owner){
    return CommandFactory::instance().createRerollDiceCommand(owner,this,this);
}

//地标卡无激活命令
QList<GameCommand*> RadioTower::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {};
}
