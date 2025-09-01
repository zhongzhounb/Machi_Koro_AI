// playerareawidget.h
#ifndef PLAYERAREAWIDGET_H
#define PLAYERAREAWIDGET_H

#include <QWidget>
#include <QList> // 确保包含 QList
#include "card.h"
#include <QBoxLayout>
class Player;
class Card;
class SlotWidget;
class QHBoxLayout;
class QVBoxLayout;
class QScrollArea;

class PlayerAreaWidget : public QWidget
{
    Q_OBJECT // 必须添加此宏

public:
    explicit PlayerAreaWidget(Player* player, bool isHBoxLayout, bool isLandMark, QWidget* parent = nullptr, bool isSelf = false);
    ~PlayerAreaWidget();

public slots:
    void onCardAdded(Player* player, Card* card); // 当卡牌添加到玩家时调用的槽
    void onCardDeled(Player* player, Card* card); // 当卡牌添加到玩家时调用的槽

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    // 新增：处理卡牌状态改变的槽函数
    void onCardStateChanged(Card* card,State state);

private:
    // 新增：辅助函数，用于根据规则更新卡槽的可见性
    //void updateSlotVisibility(SlotWidget* slotWidget);

    Player* m_player;
    bool m_isHBoxLayout;
    bool m_isLandMark;
    bool m_isSelf;

    QScrollArea* m_scrollArea;
    QWidget* m_contentWidget; // QScrollArea 的内容容器
    QBoxLayout* m_cardLayout;    // 卡牌的布局 (QHBoxLayout 或 QVBoxLayout)

    QList<SlotWidget*> m_slots; // 用于跟踪所有 SlotWidget 的列表
};

#endif // PLAYERAREAWIDGET_H
