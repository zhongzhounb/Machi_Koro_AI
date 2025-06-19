#ifndef STEALCOINSFROMALLCOMMAND_H
#define STEALCOINSFROMALLCOMMAND_H
#include "gamecommand.h"

class StealCoinsFromAllCommand: public GameCommand
{
public:
    explicit StealCoinsFromAllCommand(Player* player, Card* card, QObject* parent = nullptr, bool isAnimation = false, const QString& description = "");
    virtual ~StealCoinsFromAllCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    QMap<int,QList<QString>> m_coinsToPlayers;//存储收了x钱的人有哪些
    int m_coinsSum; // 存储最终要获得的金币数量
};
#endif // STEALCOINSFROMALLCOMMAND_H
