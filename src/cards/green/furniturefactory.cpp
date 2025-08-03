#include "furniturefactory.h"
#include "player.h"
FurnitureFactory::FurnitureFactory(QObject* parent):
    Card("家具工厂", 3, Color::Green, Type::Factory, 3 , 8, 8,CommandType::GainCoins, parent
           ,"拥有%1个"+typeToImg(Type::Industry)){}

QString FurnitureFactory::getDescription() const {
    return QString("若你是投掷者，你每拥有一个%1，获得 %2 金币。").arg(typeToImg(Type::Industry)).arg(m_value);
}

int FurnitureFactory::getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const{
    int num=owner->getTypeCardNum(Type::Industry,State::None);
    return num;
}
