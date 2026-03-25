#ifndef INITGAMECOMMAND_H
#define INITGAMECOMMAND_H

#include "gamecommand.h"

class InitGameCommand : public GameCommand
{
    Q_OBJECT
public:
    explicit InitGameCommand(Player* sourcePlayer = nullptr, QObject* parent = nullptr,
                             QList<Card*> cards = {}, Player* activePlayer = nullptr);
    virtual ~InitGameCommand() = default;

    // 获取提示数据：触发 UI 加载动画
    PromptData getPromptData(GameState* state) const override;

    // 设置输入：接收 UI 回调并触发逻辑执行
    bool setInput(int optionId, GameState* state, GameController* controller = nullptr) override;

    // 执行逻辑：进行真正的初始化并修改全局变量
    void execute(GameState* state, GameController* controller = nullptr) override;
};

#endif // INITGAMECOMMAND_H
