#ifndef COMMANDFACTORY_H
#define COMMANDFACTORY_H

#include"global.h"

// 前向声明所有需要工厂创建的类或在函数签名中使用的类/枚举
// 这样做可以避免在头文件中包含所有具体的命令类头文件，减少编译依赖
class GameCommand;
class Player;
class GameState; // 需要 GameState 来帮助恢复 Player*
class Card;

class CommandFactory {
public:
    // 单例模式，获取工厂实例
    static CommandFactory& instance();

    // 初始化游戏命令
    GameCommand* createInitGameCommand(QObject* parent=nullptr);

    // 游戏开始命令
    GameCommand* createStartTurnCommand(Player* sourcePlayer, QObject* parent=nullptr);

    // ⚠第一步，抛骰子相关命令
    GameCommand* createRollDiceCommand(Player* sourcePlayer, QObject* parent=nullptr);
    GameCommand* createRerollDiceCommand(Player* sourcePlayer, Card* card, QObject* parent=nullptr, bool isFailed=false, const QString& failureMessage="");
    GameCommand* createAddDiceNumCommand(Player* sourcePlayer, Card* card, QObject* parent=nullptr, bool isFailed=false, const QString& failureMessage="");

    // ⚠第二步，执行卡牌效果相关命令
    GameCommand* createActivateCardsCommand(Player* sourcePlayer,QObject* parent=nullptr);

    /*红卡效果*/
    GameCommand* createStealCoinsCommand(Player* sourcePlayer,Card* card,Player* activePlayer,  QObject* parent=nullptr, bool isFailed=false, const QString& failureMessage="");

    /*蓝绿卡效果*/
    GameCommand* createGainCoinsCommand(Player* sourcePlayer, Card* card, QObject* parent=nullptr, bool isFailed=false, const QString& failureMessage="");
    GameCommand* createGainCoinsWithDicesCommand(Player* sourcePlayer, Card* card, QObject* parent=nullptr, bool isFailed=false, const QString& failureMessage="");
    GameCommand* createGainCoinsComboNameCommand(Player* sourcePlayer, Card* card, const QString comboName,QObject* parent=nullptr, bool isFailed=false, const QString& failureMessage="");
    GameCommand* createGainCoinsComboTypeCommand(Player* sourcePlayer, Card* card, Type comboType,bool isSelf,QObject* parent=nullptr, bool isFailed=false, const QString& failureMessage="");
    GameCommand* createCloseLandmarkCommand(Player* sourcePlayer, Card* card, QObject* parent=nullptr, bool isFailed=false, const QString& failureMessage="");
    GameCommand* createGiveCardCommand(Player* sourcePlayer, Card* card, QObject* parent=nullptr, bool isFailed=false, const QString& failureMessage="");

    /*紫卡效果*/
    GameCommand* createStealCoinsFromAllCommand(Player* sourcePlayer, Card* card, QObject* parent=nullptr, bool isFailed=false, const QString& failureMessage="");
    GameCommand* createStealCoinsHalfFromWealthyCommand(Player* sourcePlayer, Card* card, QObject* parent=nullptr, bool isFailed=false, const QString& failureMessage="");
    GameCommand* createStealCoinsComboRedStoreCommand(Player* sourcePlayer, Card* card, QObject* parent=nullptr, bool isFailed=false, const QString& failureMessage="");
    GameCommand* createSwapCardCommand(Player* sourcePlayer, Card* card,QObject* parent=nullptr, bool isFailed=false, const QString& failureMessage="");

    // 第三步，买卡相关命令
    GameCommand* createGainOneCoinIfNoCoinsCommand(Player* sourcePlayer, QObject* parent=nullptr);
    GameCommand* createBuyCardCommand(Player* sourcePlayer, QObject* parent=nullptr);
    GameCommand* createGainCoinsIfNoBuyCardCommand(Player* sourcePlayer, Card* card, QObject* parent=nullptr, bool isFailed=false, const QString& failureMessage="");
    GameCommand* createInvestTechnologyCompanyCommand(Player* sourcePlayer, Card* card, QObject* parent=nullptr, bool isFailed=false, const QString& failureMessage="");
    GameCommand* createGainNewTurnCommand(Player* sourcePlayer, Card* card, QObject* parent=nullptr, bool isFailed=false, const QString& failureMessage="");

private:
    // 私有构造函数和删除拷贝构造/赋值运算符，确保单例模式
    CommandFactory() = default;
    CommandFactory(const CommandFactory&) = delete;
    CommandFactory& operator=(const CommandFactory&) = delete;
};

#endif // COMMANDFACTORY_H
