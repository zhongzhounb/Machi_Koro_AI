#include "wheatfield.h"
#include "commandfactory.h"

WheatField::WheatField(QObject* parent):
    Card("麦田", 1, Color::Blue, Type::Agriculture, 1 , 1, 1 ,parent) {}

QString WheatField::getDescription() const {
    return QString("获得 %1 金币。").arg(this->m_value);
}

double WheatField::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> WheatField::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {CommandFactory::instance().createGainCoinsCommand(owner,this,this)};
}
