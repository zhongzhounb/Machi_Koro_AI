#include "cheesefactory.h"
#include "player.h"
CheeseFactory::CheeseFactory(QObject* parent):
    Card("奶酪工厂", 5, Color::Green, Type::Factory, 3 , 7, 7,CommandType::GainCoins,parent
           ,"拥有%1个"+typeToImg(Type::Husbandry)) {}

QString CheeseFactory::getDescription() const {
    return QString("若你是投掷者，你每拥有一个%1，获得 %2 金币。").arg(typeToImg(Type::Husbandry)).arg(m_value);
}

int CheeseFactory::getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const{
    int num=owner->getTypeCardNum(Type::Husbandry,State::None);
    return num;
}


