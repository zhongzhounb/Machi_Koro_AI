#include "sushibar.h"
#include "commandfactory.h"
#include "player.h"

SushiBar::SushiBar(QObject* parent):
    Card("寿司店", 2, Color::Red, Type::Restaurant, 3 , 1, 1,parent) {}

QString SushiBar::getDescription() const {
    return QString("若你建成【港口】，收取投掷者 %1 金币。").arg(this->m_value);
}

int SushiBar::getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const{
    int num=owner->getCardNum("港口",State::Opening);
    if(num>=1)
        return 1;
    return 0;
}

double SushiBar::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> SushiBar::createCommands(Player* owner, Player* activePlayer){
    //如果没有建港口，激活失败
    if(owner->getCardNum("港口",State::Opening)==0)
        return { CommandFactory::instance().createStealCoinsCommand(owner,this,activePlayer,this,true,"没建【港口】，无法运输寿司店所需的海产品。")};

    return { CommandFactory::instance().createStealCoinsCommand(owner,this,activePlayer,this) };
}
