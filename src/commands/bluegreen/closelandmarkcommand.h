#ifndef CLOSELANDMARKCOMMAND_H
#define CLOSELANDMARKCOMMAND_H
#include "gamecommand.h"

class CloseLandmarkCommand: public GameCommand
{
public:
    explicit CloseLandmarkCommand(Player* player, Card* card, QObject* parent = nullptr, bool isFailed = false, const QString& failureMessage = "");
    virtual ~CloseLandmarkCommand()= default;

    // 检查是否需要用户交互（默认不需要交互）
    bool requiresUserInput() const override;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    int m_landmarkNum;//当前地标总数
    int m_coinsSum; // 存储最终要获得的金币数量
    int m_cardNum;       // 存储触发卡牌的数量 (例如，麦田*3 中的 3)
    QList<QString> m_closeNames;//拆除的名字
};

#endif // CLOSELANDMARKCOMMAND_H
