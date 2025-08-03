#include "shoppingmall.h"

ShoppingMall::ShoppingMall(QObject* parent):
    Card("购物中心", 10, Color::Landmark, Type::Landmark, -1 , 0, 0,CommandType::None, parent
           ,""
           ,""
           ,State::Closing
           ,CommandType::None) {}

QString ShoppingMall::getDescription() const {
    return QString("你的每张拥有%1或%2标志的卡收益+1。");
}
