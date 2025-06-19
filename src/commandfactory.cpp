#include "commandfactory.h"
#include "commands/gaincoinscommand.h"
#include "commands/stealcoinscommand.h"
#include "commands/gaincoinscombonamecommand.h"
#include "commands/gaincoinscombotypecommand.h"

CommandFactory& CommandFactory::instance() {
    static CommandFactory factory;
    return factory;
}

GameCommand* CommandFactory::createGainCoinsCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage) {
    return new GainCoinsCommand(sourcePlayer, card, parent, isFailed, failureMessage);
}

GameCommand* CommandFactory::createStealCoinsCommand(Player* sourcePlayer, Card* card,Player* activePlayer, QObject* parent, bool isFailed, const QString& failureMessage){
    return new StealCoinsCommand(sourcePlayer,card,activePlayer,parent, isFailed, failureMessage);
}

GameCommand* createGainCoinsComboNameCommand(Player* sourcePlayer, Card* card,const QString comboName, QObject* parent, bool isFailed, const QString& failureMessage){
    return new GainCoinsComboNameCommand(sourcePlayer,card,comboName,parent, isFailed, failureMessage);
}

GameCommand* createGainCoinsComboTypeCommand(Player* sourcePlayer, Card* card,Type comboType, QObject* parent,bool isSelf, bool isFailed, const QString& failureMessage){
    return new GainCoinsComboTypeCommand(sourcePlayer,card,comboType,parent,isSelf, isFailed, failureMessage);
}
