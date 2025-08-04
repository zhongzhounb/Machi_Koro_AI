#ifndef STEALCOINSFROMALLCOMMAND_H
#define STEALCOINSFROMALLCOMMAND_H
#include "gamecommand.h"

class StealCoinsFromAllCommand: public GameCommand
{
    Q_OBJECT
public:
    explicit StealCoinsFromAllCommand(Player* player, Card* card, QObject* parent = nullptr);
    virtual ~StealCoinsFromAllCommand()= default;

    // 执行命令的核心逻辑。此方法假定 m_userChoice 已经设置。为了方便存储计算结果，就不存储，直接输出日志。
    void execute(GameState* state, GameController* controller=nullptr) override;
};
#endif // STEALCOINSFROMALLCOMMAND_H
