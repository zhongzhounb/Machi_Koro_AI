#include "commandfactory.h"
#include "gaincoinscommand.h"
#include "stealcoinscommand.h"
#include "gaincoinscombonamecommand.h"
#include "gaincoinscombotypecommand.h"
#include "stealcoinsfromallcommand.h"
#include "stealcoinshalffromwealthycommand.h"
#include "stealcoinscomboredstorecommand.h"

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

GameCommand* CommandFactory::createGainCoinsComboNameCommand(Player* sourcePlayer, Card* card,const QString comboName, QObject* parent, bool isFailed, const QString& failureMessage){
    return new GainCoinsComboNameCommand(sourcePlayer,card,comboName,parent, isFailed, failureMessage);
}

GameCommand* CommandFactory::createGainCoinsComboTypeCommand(Player* sourcePlayer, Card* card,Type comboType, QObject* parent,bool isSelf, bool isFailed, const QString& failureMessage){
    return new GainCoinsComboTypeCommand(sourcePlayer,card,comboType,parent,isSelf, isFailed, failureMessage);
}

GameCommand* CommandFactory::createStealCoinsFromAllCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage) {
    return new StealCoinsFromAllCommand(sourcePlayer, card, parent, isFailed, failureMessage);
}

GameCommand* CommandFactory::createStealCoinsHalfFromWealthyCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage) {
    return new StealCoinsHalfFromWealthyCommand(sourcePlayer, card, parent, isFailed, failureMessage);
}

GameCommand* CommandFactory::createStealCoinsComboRedStoreCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage) {
    return new StealCoinsComboRedStoreCommand(sourcePlayer, card, parent, isFailed, failureMessage);
}


