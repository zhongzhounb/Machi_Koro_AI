#include "publisher.h"

Publisher::Publisher(QObject* parent):
    Card("出版社", 5, Color::Purple, Type::Office, -1 , 7, 7,CommandType::StealCoinsFromAll, parent) {}

QString Publisher::getDescription() const {
    return QString("若你是投掷者，其他人每拥有一个%1/%2，收取其 1 金币。").arg(typeToImg(Type::Restaurant).arg(typeToImg(Type::Store)));
}

int Publisher::getStealCoins(Player* player)const{
    int num=player->getTypeCardNum(Type::Store,State::None)+player->getTypeCardNum(Type::Restaurant,State::None);
    return num;
}
