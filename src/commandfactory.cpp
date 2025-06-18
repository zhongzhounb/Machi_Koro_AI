#include "commandfactory.h"
#include "commands/gaincoinscommand.h"
#include "commands/stealcoinscommand.h"

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
