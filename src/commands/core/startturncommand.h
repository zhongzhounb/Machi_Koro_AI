#ifndef STARTTURNCOMMAND_H
#define STARTTURNCOMMAND_H
#include "gamecommand.h"

class StartTurnCommand:public GameCommand
{
    Q_OBJECT
public:
    explicit StartTurnCommand(Player* sourcePlayer = nullptr, QObject* parent = nullptr,
                              QList<Card*> cards={},Player* activePlayer=nullptr);
    virtual ~StartTurnCommand()= default;

    // 检查是否需要用户交互（默认不需要交互）
    PromptData getPromptData(GameState* state)const override;
    // 设置选项，返回是否要继续获得选项（无选项时禁止调用）
    bool setInput(int optionId,GameState* state, GameController* controller=nullptr) override;
    void execute(GameState* state, GameController* controller=nullptr) override;

};

#endif // STARTTURNCOMMAND_H
