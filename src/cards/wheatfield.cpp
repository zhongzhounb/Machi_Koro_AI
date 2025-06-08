#include "wheatfield.h"
#include "card.h"
WheatField::WheatField(int suffixId,QObject* parent):
    Card(100*100+suffixId,"麦田", 1, Color::Blue, Type::Agriculture, 1, 1, "当你掷出1点时，从银行获得1枚硬币。",parent) {}

double WheatField::getBuyWight(Player* aiPlayer, Game* game) const {
    return 0.0;
}

QString WheatField::activate(Player* owner, Player* activePlayer, int rollSum, Game* game, const QVariant& choiceData){
    return "";
}
