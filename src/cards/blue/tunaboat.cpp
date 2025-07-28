#include "tunaboat.h"
#include "commandfactory.h"
#include "player.h"

TunaBoat::TunaBoat(QObject* parent):
    Card("金枪鱼船", 5, Color::Blue, Type::Fishery,7, 12, 14, parent) {}

QString TunaBoat::getDescription() const {
    return QString("若你建成【港口】，将投掷两个骰子，获得两骰子之和的金币。").arg(this->m_value);
}

int TunaBoat::getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const{
    int num=owner->getCardNum("港口",State::Opening);
    if(num>0)
        return 1;
    return 0;
}

double TunaBoat::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> TunaBoat::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    //如果没有建港口，激活失败
    if(owner->getCardNum("港口",State::Opening)==0)
        return {CommandFactory::instance().createGainCoinsWithDicesCommand(owner,this,this,true,"没建【港口】，船无法停靠。")};

    return {CommandFactory::instance().createGainCoinsWithDicesCommand(owner,this,this)};
}

