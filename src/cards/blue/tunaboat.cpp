#include "tunaboat.h"
#include "player.h"

TunaBoat::TunaBoat(QObject* parent):
    Card("金枪鱼船", 5, Color::Blue, Type::Fishery,7, 12, 14,CommandType::GainCoinsWithDices, parent
           ,""
           ,"没建【港口】，船无法停靠。") {}

QString TunaBoat::getDescription() const {
    return QString("若你建成【港口】，将投掷两个骰子，获得两骰子之和的金币。");
}

int TunaBoat::getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const{
    int num=owner->getCardNum("港口",State::Opening);
    return num>0;
}
