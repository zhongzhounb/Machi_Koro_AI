#ifndef BUYCARDCOMMAND_H
#define BUYCARDCOMMAND_H
#include "gamecommand.h"

class BuyCardCommand: public GameCommand
{
    Q_OBJECT
public:
    explicit BuyCardCommand(Player* player,  QObject* parent = nullptr);
    virtual ~BuyCardCommand()= default;

    // 检查是否需要用户交互（默认不需要交互）
    PromptData getPromptData(GameState* state) override;
    // 获取默认选项（无选项时禁止调用）
    int getAutoInput( const PromptData& promptData ,GameState* state) override;
    // 设置选项，返回是否要继续获得选项（无选项时禁止调用）
    bool setInput(int optionId,GameState* state) override;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    QString m_cardName;
    int m_cardCoins;

};

#endif // BUYCARDCOMMAND_H
