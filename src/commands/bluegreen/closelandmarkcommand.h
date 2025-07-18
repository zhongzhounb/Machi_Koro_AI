#ifndef CLOSELANDMARKCOMMAND_H
#define CLOSELANDMARKCOMMAND_H
#include "gamecommand.h"

class CloseLandmarkCommand: public GameCommand
{
public:
    explicit CloseLandmarkCommand(Player* player, Card* card, QObject* parent = nullptr, bool isFailed = false, const QString& failureMessage = "");
    virtual ~CloseLandmarkCommand()= default;

    // 检查是否需要用户交互（默认不需要交互）
    PromptData getPromptData(GameState* state) override;
    // 获取默认选项（无选项时禁止调用）
    int getAutoInput( const PromptData& promptData ,GameState* state) override;
    // 设置选项，返回是否要继续获得选项（无选项时禁止调用）
    bool setInput(int optionId,GameState* state) override;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    int m_landmarkNum;//当前地标总数
    int m_coinsSum; // 存储最终要获得的金币数量
    int m_cardNum;       // 存储触发卡牌的数量 (例如，麦田*3 中的 3)
    QList<QString> m_closeNames;//拆除的名字
};

#endif // CLOSELANDMARKCOMMAND_H
