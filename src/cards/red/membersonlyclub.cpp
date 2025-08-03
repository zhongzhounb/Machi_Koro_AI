#include "membersonlyclub.h"
#include "player.h"

MembersOnlyClub::MembersOnlyClub(QObject* parent):
    Card("会员俱乐部", 4, Color::Red, Type::Restaurant, 999 , 12, 14,CommandType::StealCoins, parent
           ,""
           ,"对方未建成≥3个地标。") {}

QString MembersOnlyClub::getDescription() const {
    return QString("若投掷者建成≥3个地标，收取投掷者全部金币。");
}

int MembersOnlyClub::getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const{
    int num=activePlayer->getTypeCardNum(Type::Landmark,State::Opening);
    return num>=3;
}
