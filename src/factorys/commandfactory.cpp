#include "commandfactory.h"
#include <QDebug> // 用于错误日志

// 构造函数：私有，确保只能通过 instance() 获取实例
CommandFactory::CommandFactory() {
    qDebug() << "CommandFactory instance created.";
}

// 实现单例模式的实例获取方法
CommandFactory& CommandFactory::instance() {
    static CommandFactory factory; // 局部静态变量，在第一次调用时初始化
    return factory;
}

// 实现 createCommand 方法，根据传入的 CommandType 创建对应的命令实例
GameCommand* CommandFactory::createCommand(CommandType type, Player* sourcePlayer, QObject* parent,
                                           QList<Card*> cards, Player* activePlayer) {
    switch (type) {
    case InitGame:
        // 注意：这里不再将 'type' 作为参数传递给 InitGameCommand 的构造函数
        return new InitGameCommand(sourcePlayer, parent, cards, activePlayer);
    case StartTurn:
        return new StartTurnCommand(sourcePlayer, parent, cards, activePlayer);
    case RollDice:
        return new RollDiceCommand(sourcePlayer, parent, cards, activePlayer);
    case RerollDice:
        return new RerollDiceCommand(sourcePlayer, parent, cards, activePlayer);
    case AddDiceNum:
        return new AddDiceNumCommand(sourcePlayer, parent, cards, activePlayer);
    case ActivateCards:
        return new ActivateCardsCommand(sourcePlayer, parent, cards, activePlayer);
    case StealCoins:
        return new StealCoinsCommand(sourcePlayer, parent, cards, activePlayer);
    case GainCoins:
        return new GainCoinsCommand(sourcePlayer, parent, cards, activePlayer);
    case GainCoinsWithDices:
        return new GainCoinsWithDicesCommand(sourcePlayer, parent, cards, activePlayer);
    case CloseLandmark:
        return new CloseLandmarkCommand(sourcePlayer, parent, cards, activePlayer);
    case GiveCard:
        return new GiveCardCommand(sourcePlayer, parent, cards, activePlayer);
    case StealCoinsFromAll:
        return new StealCoinsFromAllCommand(sourcePlayer, parent, cards, activePlayer);
    case SwapCard:
        return new SwapCardCommand(sourcePlayer, parent, cards, activePlayer);
    case GainOneCoinIfNoCoins:
        return new GainOneCoinIfNoCoinsCommand(sourcePlayer, parent, cards, activePlayer);
    case BuyCard:
        return new BuyCardCommand(sourcePlayer, parent, cards, activePlayer);
    case InvestTechnologyCompany:
        return new InvestTechnologyCompanyCommand(sourcePlayer, parent, cards, activePlayer);
    case GainNewTurn:
        return new GainNewTurnCommand(sourcePlayer, parent, cards, activePlayer);
    case None:
    default:
        qWarning("CommandFactory: Unhandled command type %d. Returning nullptr.", static_cast<int>(type));
        return nullptr;
    }
}
