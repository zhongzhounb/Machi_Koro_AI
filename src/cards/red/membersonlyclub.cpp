#include "membersonlyclub.h"
#include "commandfactory.h"
#include "player.h"

MembersOnlyClub::MembersOnlyClub(QObject* parent):
    Card("会员俱乐部", 4, Color::Red, Type::Restaurant, 999 , 12, 14, parent) {}

QString MembersOnlyClub::getDescription() const {
    return QString("若投掷者建成≥3个地标，收取投掷者全部金币。");
}

int MembersOnlyClub::getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const{
    int num=activePlayer->getTypeCardNum(Type::Landmark,State::Opening);
    if(num>=3)
        return 1;
    return 0;
}

double MembersOnlyClub::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> MembersOnlyClub::createCommands(Player* owner, Player* activePlayer){
    //如果对方没开这么多地标，激活失败
    if(activePlayer->getTypeCardNum(Type::Landmark,State::Opening)<3)
        return { CommandFactory::instance().createStealCoinsCommand(owner,this,activePlayer,this,true ,QString("不能收 %1 的钱，因为他还没建这么多地标。").arg(activePlayer->getName()))};

    return { CommandFactory::instance().createStealCoinsCommand(owner,this,activePlayer,this) };
}
