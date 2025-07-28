#include "generalstore.h"
#include "commandfactory.h"
#include "player.h"

GeneralStore::GeneralStore(QObject* parent):
    Card("杂货店", 0, Color::Green, Type::Store, 2 , 2, 2,parent) {}

QString GeneralStore::getDescription() const {
    return QString("若你是投掷者，且建成≤1地标，获得 %1 金币。").arg(this->m_value);
}
int GeneralStore::getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const{
    int num=owner->getTypeCardNum(Type::Landmark,State::Opening);
    if(num<=1)
        return 1;
    return 0;
}


double GeneralStore::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> GeneralStore::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);

    if(activePlayer->getTypeCardNum(Type::Landmark,State::Opening)>1)
        return { CommandFactory::instance().createGainCoinsCommand(owner,this,this,true ,QString("建了太多地标了，没时间管理杂货店了。"))};

    return { CommandFactory::instance().createGainCoinsCommand(owner,this,this) };
}
