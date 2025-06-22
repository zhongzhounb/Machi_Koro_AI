#ifndef STEALCOINSCOMBOREDSTORECOMMAND_H
#define STEALCOINSCOMBOREDSTORECOMMAND_H
#include "gamecommand.h"

class StealCoinsComboRedStoreCommand: public GameCommand
{
public:
    explicit StealCoinsComboRedStoreCommand(Player* player, Card* card, QObject* parent = nullptr, bool isFailed = false, const QString& failureMessage = "");
    virtual ~StealCoinsComboRedStoreCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    QMap<int,QList<QString>> m_coinsToPlayers;//存储收了x钱的人有哪些
    int m_coinsSum; // 存储最终要获得的金币数量
};

#endif // STEALCOINSCOMBOREDSTORECOMMAND_H
