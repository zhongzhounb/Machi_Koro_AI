#ifndef GAINCOINSCOMBOTYPECOMMAND_H
#define GAINCOINSCOMBOTYPECOMMAND_H
#include "gamecommand.h"

class GainCoinsComboTypeCommand: public GameCommand
{
public:
    explicit GainCoinsComboTypeCommand(Player* player, Card* card,Type comboType, bool isSelf=true,QObject* parent = nullptr, bool isFailed=false, const QString& failureMessage = "");
    virtual ~GainCoinsComboTypeCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    Type m_comboType;// 组合类型
    bool m_isSelf;  // 和自己组合还是全场组合
    int m_coinsSum; // 存储最终要获得的金币数量
    int m_comboNum; // 组合卡数目
    int m_cardNum;  // 存储触发卡牌的数量 (例如，麦田*3 中的 3)
};

#endif // GAINCOINSCOMBOTYPECOMMAND_H
