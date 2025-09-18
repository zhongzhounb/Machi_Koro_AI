#ifndef GAINCOINSIFNOBUYCOMMAND_H
#define GAINCOINSIFNOBUYCOMMAND_H
#include "gamecommand.h"

class GainCoinsIfNoBuyCommand: public GameCommand
{
    Q_OBJECT
public:
    explicit GainCoinsIfNoBuyCommand(Player* sourcePlayer = nullptr, QObject* parent = nullptr,
                              QList<Card*> cards={},Player* activePlayer=nullptr);
    virtual ~GainCoinsIfNoBuyCommand()= default;

    // 执行命令的核心逻辑。此方法假定 m_userChoice 已经设置。为了方便存储计算结果，就不存储，直接输出日志。
    void execute(GameState* state, GameController* controller=nullptr) override;
};

#endif // GAINCOINSIFNOBUYCOMMAND_H
