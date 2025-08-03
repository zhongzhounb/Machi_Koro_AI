#ifndef GAINCOINSWITHDICESCOMMAND_H
#define GAINCOINSWITHDICESCOMMAND_H
#include "gamecommand.h"

class GainCoinsWithDicesCommand: public GameCommand
{
    Q_OBJECT
public:
    explicit GainCoinsWithDicesCommand(Player* player, Card* card, QObject* parent = nullptr, bool isFailed = false, const QString& failureMessage = "");
    virtual ~GainCoinsWithDicesCommand()= default;

    // 检查是否需要用户交互（默认不需要交互）
    PromptData getPromptData(GameState* state)const override;
    // 设置选项，返回是否要继续获得选项（无选项时禁止调用）
    bool setInput(int optionId,GameState* state, GameController* controller=nullptr) override;
    // 执行命令的核心逻辑。此方法假定 m_userChoice 已经设置。为了方便存储计算结果，就不存储，直接输出日志。
    void execute(GameState* state, GameController* controller=nullptr) override;

};

#endif // GAINCOINSWITHDICESCOMMAND_H
