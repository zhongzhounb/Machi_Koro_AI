#ifndef INVESTTECHNOLOGYCOMPANYCOMMAND_H
#define INVESTTECHNOLOGYCOMPANYCOMMAND_H
#include "gamecommand.h"

class InvestTechnologyCompanyCommand: public GameCommand
{
public:
    explicit InvestTechnologyCompanyCommand(Player* player, Card* card, QObject* parent = nullptr, bool isAnimation = false, const QString& description = "");
    virtual ~InvestTechnologyCompanyCommand()= default;

    // 检查是否需要用户交互（默认不需要交互）
    bool requiresUserInput() const override;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    int m_coinsSum; // 存储当前总投资
};


#endif // INVESTTECHNOLOGYCOMPANYCOMMAND_H
