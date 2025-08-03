#include "fruitandvegetablemarket.h"
#include "player.h"
FruitAndVegetableMarket::FruitAndVegetableMarket(QObject* parent):
    Card("果蔬超市", 2, Color::Green, Type::Factory, 2 , 11, 12,CommandType::GainCoins, parent
           ,"拥有%1个"+typeToImg(Type::Agriculture)) {}

QString FruitAndVegetableMarket::getDescription() const {
    return QString("若你是投掷者，你每拥有一个%1，获得 %2 金币。").arg(typeToImg(Type::Agriculture)).arg(m_value);
}

int FruitAndVegetableMarket::getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const{
    int num=owner->getTypeCardNum(Type::Agriculture,State::None);
    return num;
}
