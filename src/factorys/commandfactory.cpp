#include "commandfactory.h"

// 包含所有具体命令类的头文件
// 请确保这些头文件存在于你的项目中，并且类名与此处使用的匹配
#include "initgamecommand.h"
#include "startturncommand.h"
#include "rolldicecommand.h"
#include "rerolldicecommand.h"
#include "adddicenumcommand.h"
#include "activatecardscommand.h"
#include "stealcoinscommand.h"
#include "gaincoinscommand.h"
#include "gaincoinswithdicescommand.h"
#include "gaincoinscombonamecommand.h"
#include "gaincoinscombotypecommand.h"
#include "closelandmarkcommand.h"
#include "givecardcommand.h"
#include "stealcoinsfromallcommand.h"
#include "stealcoinshalffromwealthycommand.h"
#include "stealcoinscomboredstorecommand.h"
#include "swapcardcommand.h"
#include "gainonecoinifnocoinscommand.h"
#include "buycardcommand.h"
#include "gaincoinsifnobuycardcommand.h"
#include "investtechnologycompanycommand.h"
#include "gainnewturncommand.h"


CommandFactory& CommandFactory::instance() {
    static CommandFactory factory;
    return factory;
}

// 初始化游戏命令
GameCommand* CommandFactory::createInitGameCommand(QObject* parent) {
    return new InitGameCommand(parent);
}

// 游戏开始命令
GameCommand* CommandFactory::createStartTurnCommand(Player* sourcePlayer, QObject* parent) {
    return new StartTurnCommand(sourcePlayer, parent);
}

// ⚠第一步，抛骰子相关命令
GameCommand* CommandFactory::createRollDiceCommand(Player* sourcePlayer, QObject* parent) {
    return new RollDiceCommand(sourcePlayer, parent);
}

GameCommand* CommandFactory::createRerollDiceCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage) {
    return new RerollDiceCommand(sourcePlayer, card, parent, isFailed, failureMessage);
}

GameCommand* CommandFactory::createAddDiceNumCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage) {
    return new AddDiceNumCommand(sourcePlayer, card, parent, isFailed, failureMessage);
}

// ⚠第二步，执行卡牌效果相关命令
GameCommand* CommandFactory::createActivateCardsCommand(Player* sourcePlayer, QObject* parent) {
    // 假设 ActivateCardsCommand 的构造函数已适配移除 Card* card 和错误参数
    return new ActivateCardsCommand(sourcePlayer, parent);
}

/*红卡效果*/
GameCommand* CommandFactory::createStealCoinsCommand(Player* sourcePlayer, Card* card, Player* activePlayer, QObject* parent, bool isFailed, const QString& failureMessage){
    return new StealCoinsCommand(sourcePlayer, card, activePlayer, parent, isFailed, failureMessage);
}

/*蓝绿卡效果*/
GameCommand* CommandFactory::createGainCoinsCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage) {
    return new GainCoinsCommand(sourcePlayer, card, parent, isFailed, failureMessage);
}

GameCommand* CommandFactory::createGainCoinsWithDicesCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage) {
    // 假设 GainCoinsWithDicesCommand 的构造函数已适配移除 diceRoll 参数
    return new GainCoinsWithDicesCommand(sourcePlayer, card, parent, isFailed, failureMessage);
}

GameCommand* CommandFactory::createGainCoinsComboNameCommand(Player* sourcePlayer, Card* card, const QString comboName, QObject* parent, bool isFailed, const QString& failureMessage) {
    return new GainCoinsComboNameCommand(sourcePlayer, card, comboName, parent, isFailed, failureMessage);
}

GameCommand* CommandFactory::createGainCoinsComboTypeCommand(Player* sourcePlayer, Card* card, Type comboType, bool isSelf, QObject* parent, bool isFailed, const QString& failureMessage) {
    return new GainCoinsComboTypeCommand(sourcePlayer, card, comboType, isSelf, parent, isFailed, failureMessage);
}

GameCommand* CommandFactory::createCloseLandmarkCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage) {
    // 假设 CloseLandmarkCommand 的构造函数已适配移除 landmarkToClose 参数
    return new CloseLandmarkCommand(sourcePlayer, card, parent, isFailed, failureMessage);
}

GameCommand* CommandFactory::createGiveCardCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage) {
    // 假设 GiveCardCommand 的构造函数已适配移除 targetPlayer 和 cardToGive 参数
    return new GiveCardCommand(sourcePlayer, card, parent, isFailed, failureMessage);
}

/*紫卡效果*/
GameCommand* CommandFactory::createStealCoinsFromAllCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage) {
    return new StealCoinsFromAllCommand(sourcePlayer, card, parent, isFailed, failureMessage);
}

GameCommand* CommandFactory::createStealCoinsHalfFromWealthyCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage) {
    return new StealCoinsHalfFromWealthyCommand(sourcePlayer, card, parent, isFailed, failureMessage);
}

GameCommand* CommandFactory::createStealCoinsComboRedStoreCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage) {
    return new StealCoinsComboRedStoreCommand(sourcePlayer, card, parent, isFailed, failureMessage);
}

GameCommand* CommandFactory::createSwapCardCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage) {
    // 假设 SwapCardCommand 的构造函数已适配移除 targetPlayer, sourcePlayerCardToSwap, targetPlayerCardToSwap 参数
    return new SwapCardCommand(sourcePlayer, card, parent, isFailed, failureMessage);
}

// 第三步，买卡相关命令
GameCommand* CommandFactory::createGainOneCoinIfNoCoinsCommand(Player* sourcePlayer, QObject* parent) {
    return new GainOneCoinIfNoCoinsCommand(sourcePlayer, parent);
}

GameCommand* CommandFactory::createBuyCardCommand(Player* sourcePlayer, QObject* parent) {
    // 假设 BuyCardCommand 的构造函数已适配移除 cardToBuy 参数
    return new BuyCardCommand(sourcePlayer, parent);
}

GameCommand* CommandFactory::createGainCoinsIfNoBuyCardCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage) {
    return new GainCoinsIfNoBuyCardCommand(sourcePlayer, card, parent, isFailed, failureMessage);
}

GameCommand* CommandFactory::createInvestTechnologyCompanyCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage) {
    return new InvestTechnologyCompanyCommand(sourcePlayer, card, parent, isFailed, failureMessage);
}

GameCommand* CommandFactory::createGainNewTurnCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage) {
    return new GainNewTurnCommand(sourcePlayer, card, parent, isFailed, failureMessage);
}
