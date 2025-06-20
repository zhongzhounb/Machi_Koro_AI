#ifndef ADDDICENUM_H
#define ADDDICENUM_H
#include "gamecommand.h"

class AddDiceNum: public GameCommand
{
public:
    explicit AddDiceNum(Player* player, Card* card, QObject* parent = nullptr, bool isAnimation = false, const QString& description = "");
    virtual ~AddDiceNum()= default;

    // 需要实时检查
    bool isValid() const override;
    // 检查是否需要用户交互（默认不需要交互）
    bool requiresUserInput() const override;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    bool m_isAdd;
};

#endif // ADDDICENUM_H
