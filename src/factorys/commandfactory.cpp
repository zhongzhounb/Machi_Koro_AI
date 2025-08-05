#include "commandfactory.h"
#include <QDebug> // 用于错误日志
// bluegreen commands
#include "closelandmarkcommand.h"
#include "gaincoinscommand.h"
#include "bluegreen/gaincoinswithdicescommand.h"
#include "bluegreen/givecardcommand.h"

// core commands
#include "core/activatecardscommand.h"
#include "core/buycardcommand.h"
#include "core/gainonecoinifnocoinscommand.h"
#include "core/initgamecommand.h"
#include "core/rolldicecommand.h"
#include "core/startturncommand.h"

// landmark commands
#include "landmark/adddicenumcommand.h"
#include "landmark/gainnewturncommand.h"
#include "landmark/rerolldicecommand.h"

// purple commands
#include "purple/investtechnologycompanycommand.h"
#include "purple/stealcoinsfromallcommand.h"
#include "purple/swapcardcommand.h"

// red commands
#include "red/stealcoinscommand.h"
// 构造函数：私有，确保只能通过 instance() 获取实例
CommandFactory::CommandFactory() {
    // 可以在这里进行任何一次性初始化，如果需要的话
    qDebug() << "CommandFactory instance created.";
}

// 实现单例模式的实例获取方法
CommandFactory& CommandFactory::instance() {
    static CommandFactory factory; // 局部静态变量，在第一次调用时初始化
    return factory;
}

// 实现 createCommand 方法，根据传入的 CommandType 创建对应的命令实例
GameCommand* CommandFactory::createCommand(CommandType type, Player* sourcePlayer, QObject* parent,
                                           Card* card, Player* activePlayer) {
    switch (type) {
    case InitGame:
        return new InitGameCommand(type, sourcePlayer, parent, card, activePlayer);
    case StartTurn:
        return new StartTurnCommand(type, sourcePlayer, parent, card, activePlayer);
    case RollDice:
        return new RollDiceCommand(type, sourcePlayer, parent, card, activePlayer);
    case RerollDice:
        return new RerollDiceCommand(type, sourcePlayer, parent, card, activePlayer);
    case AddDiceNum:
        return new AddDiceNumCommand(type, sourcePlayer, parent, card, activePlayer);
    case ActivateCards:
        return new ActivateCardsCommand(type, sourcePlayer, parent, card, activePlayer);
    case StealCoins:
        return new StealCoinsCommand(type, sourcePlayer, parent, card, activePlayer);
    case GainCoins:
        return new GainCoinsCommand(type, sourcePlayer, parent, card, activePlayer);
    case GainCoinsWithDices:
        return new GainCoinsWithDicesCommand(type, sourcePlayer, parent, card, activePlayer);
    case CloseLandmark:
        return new CloseLandmarkCommand(type, sourcePlayer, parent, card, activePlayer);
    case GiveCard:
        return new GiveCardCommand(type, sourcePlayer, parent, card, activePlayer);
    case StealCoinsFromAll:
        return new StealCoinsFromAllCommand(type, sourcePlayer, parent, card, activePlayer);
    case SwapCard:
        return new SwapCardCommand(type, sourcePlayer, parent, card, activePlayer);
    case GainOneCoinIfNoCoins:
        return new GainOneCoinIfNoCoinsCommand(type, sourcePlayer, parent, card, activePlayer);
    case BuyCard:
        return new BuyCardCommand(type, sourcePlayer, parent, card, activePlayer);
    case InvestTechnologyCompany:
        return new InvestTechnologyCompanyCommand(type, sourcePlayer, parent, card, activePlayer);
    case GainNewTurn:
        return new GainNewTurnCommand(type, sourcePlayer, parent, card, activePlayer);
    case None:
    default:
        qWarning("CommandFactory: Unhandled command type %d. Returning nullptr.", static_cast<int>(type));
        return nullptr;
    }
}
