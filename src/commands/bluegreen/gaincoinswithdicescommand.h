#ifndef GAINCOINSWITHDICESCOMMAND_H
#define GAINCOINSWITHDICESCOMMAND_H
#include "gamecommand.h"

class GainCoinsWithDicesCommand: public GameCommand
{
    Q_OBJECT
public:
    explicit GainCoinsWithDicesCommand(Player* sourcePlayer = nullptr, QObject* parent = nullptr,
                                       QList<Card*> cards={},Player* activePlayer=nullptr);
    virtual ~GainCoinsWithDicesCommand()= default;

    // 检查是否需要用户交互（默认不需要交互）
    PromptData getPromptData(GameState* state)const override;
    // 执行命令的核心逻辑。此方法假定 m_userChoice 已经设置。为了方便存储计算结果，就不存储，直接输出日志。
    void execute(GameState* state, GameController* controller=nullptr) override;
};

#endif // GAINCOINSWITHDICESCOMMAND_H
