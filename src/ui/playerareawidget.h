// playerareawidget.h

#ifndef PLAYERAREAWIDGET_H
#define PLAYERAREAWIDGET_H

#include <QWidget>
#include <QList>

// 前向声明，避免在头文件中包含重量级的头文件
class Player;
class Card;
class SlotWidget;
class CardWidget;
class QScrollArea;
class QBoxLayout;

class PlayerAreaWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerAreaWidget(Player* player, bool isHBoxLayout, bool isLandMark, QWidget* parent = nullptr);
    ~PlayerAreaWidget();



public slots:
    void onCardAdded(Player* player, Card* card);

protected:
    void resizeEvent (QResizeEvent* event) override;

private:
    // 数据
    Player* m_player;
    bool m_isHBoxLayout;
    bool m_isLandMark;

    // 布局结构
    QScrollArea* m_scrollArea;          // 滚动区域
    QWidget* m_contentWidget;           // ScrollArea的直接子控件，会被拉伸，用于居中
    QWidget* m_cardContainer;           // 中间容器，不会被拉伸，容纳所有卡牌
    QBoxLayout* m_cardLayout;           // 真正排列卡牌的布局，位于 m_cardContainer 中


    // 状态
    QList<SlotWidget*> m_slots;

};

#endif // PLAYERAREAWIDGET_H
