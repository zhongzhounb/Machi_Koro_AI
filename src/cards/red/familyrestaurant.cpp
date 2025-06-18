#include "familyrestaurant.h"
#include "commandfactory.h"
#include "player.h"

FamilyRestaurant::FamilyRestaurant(QObject* parent):
    Card("西餐厅", 3, Color::Red, Type::Restaurant, 2 , 9, 10, parent) {}

QString FamilyRestaurant::getDescription() const {
    return QString("收取投掷者 %1 金币。").arg(this->m_value);
}

double FamilyRestaurant::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> FamilyRestaurant::createCommands(Player* owner, Player* activePlayer){
    return { CommandFactory::instance().createStealCoinsCommand(owner,this,activePlayer,this) };
}
