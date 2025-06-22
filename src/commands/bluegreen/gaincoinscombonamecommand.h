#ifndef GAINCOINSCOMBONAMECOMMAND_H
#define GAINCOINSCOMBONAMECOMMAND_H
#include "gamecommand.h"

class GainCoinsComboNameCommand: public GameCommand
{
public:
    explicit GainCoinsComboNameCommand(Player* player, Card* card,const QString comboName, QObject* parent = nullptr, bool isFailed = false, const QString& failureMessage = "");
    virtual ~GainCoinsComboNameCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    QString m_comboName;
    int m_coinsSum; // 存储最终要获得的金币数量
    int m_comboNum; // 组合卡数目
    int m_cardNum;  // 存储触发卡牌的数量 (例如，麦田*3 中的 3)
};


#endif // GAINCOINSCOMBONAMECOMMAND_H
