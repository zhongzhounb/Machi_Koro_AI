#include "frenchrestaurant.h"
#include "commandfactory.h"
#include "player.h"

FrenchRestaurant::FrenchRestaurant(QObject* parent):
    Card("法国餐厅", 3, Color::Red, Type::Restaurant, 5 , 5, 5, parent) {}

QString FrenchRestaurant::getDescription() const {
    return QString("若投掷者建造了≥2个地标，收取投掷者 %1 金币。").arg(this->m_value);
}

double FrenchRestaurant::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> FrenchRestaurant::createCommands(Player* owner, Player* activePlayer){
    //如果对方没开这么多地标，激活失败
    if(activePlayer->getTypeCardNum(Type::Landmark,State::Opening)<2)
        return { CommandFactory::instance().createStealCoinsCommand(owner,this,activePlayer,this,true ,QString("不能收 %1 的钱，因为他还没建这么多地标。").arg(activePlayer->getName()))};

    return { CommandFactory::instance().createStealCoinsCommand(owner,this,activePlayer,this) };
}
