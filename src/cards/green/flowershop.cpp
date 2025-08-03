#include "flowershop.h"
#include "player.h"
FlowerShop::FlowerShop(QObject* parent):
    Card("花店", 1, Color::Green, Type::Store, 1 , 6, 6,CommandType::GainCoins, parent
           ,"拥有%1个【花田】") {}

QString FlowerShop::getDescription() const {
    return QString("若你是投掷者，你每拥有一个【花田】，获得 %1 金币。").arg(m_value);
}

int FlowerShop::getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const{
    int num=owner->getCardNum("花田",State::None);
    return num;
}
