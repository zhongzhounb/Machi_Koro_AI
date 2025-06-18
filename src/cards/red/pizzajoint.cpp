#include "pizzajoint.h"
#include "commandfactory.h"
#include "player.h"

PizzaJoint::PizzaJoint(QObject* parent):
    Card("披萨店", 1, Color::Red, Type::Restaurant, 1 , 7, 7, parent) {}

QString PizzaJoint::getDescription() const {
    return QString("收取投掷者 %1 金币。").arg(this->m_value);
}

double PizzaJoint::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> PizzaJoint::createCommands(Player* owner, Player* activePlayer){
    return { CommandFactory::instance().createStealCoinsCommand(owner,this,activePlayer,this) };
}
