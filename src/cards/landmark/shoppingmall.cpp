#include "shoppingmall.h"
#include "commandfactory.h"

ShoppingMall::ShoppingMall(QObject* parent):
    Card("购物中心", 10, Color::Landmark, Type::Landmark, -1 , 0, 0, parent,State::Closing) {}

QString ShoppingMall::getDescription() const {
    return QString("你的每张拥有%1或%2标志的卡收益+1。");
}

double ShoppingMall::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

//购物中心作为单次生效卡牌，作用在状态中修改
QList<GameCommand*> ShoppingMall::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(owner);Q_UNUSED(activePlayer);
    return {};
}
