#ifndef PLAYERAREAWIDGET_H
#define PLAYERAREAWIDGET_H

#include <QWidget>
#include <QList>
#include <QMap>
#include <QBoxLayout> // 包含 QBoxLayout 头文件
#include <QPoint> // 新增：QPoint 头文件

// 前向声明
class Player;
class Card;
class SlotWidget;
class CardWidget;
class QScrollArea;

enum class State;

class PlayerAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerAreaWidget(Player* player, bool isHBoxLayout, bool isLandMark, QWidget* parent = nullptr, bool isSelf = false);
    ~PlayerAreaWidget();

    QPoint getCardTargetSlotCenterPos(Card* card);
    QSize getTargetCardSize();

signals:
    // 新增：转发 CardWidget 的请求显示详细卡牌信号
    void cardWidgetRequestShowDetailed(Card* card, QPoint globalPos);
    // 新增：转发 CardWidget 的请求隐藏详细卡牌信号
    void cardWidgetRequestHideDetailed();

private slots:
    void onCardAdded(Player* player, Card* card);
    void onCardDeled(Player* player, Card* card);
    void onCardStateChanged(Card* card, State state);
    // 新增：处理内部 SlotWidget 的请求
    void handleSlotWidgetRequestShowDetailed(Card* card, QPoint globalPos);
    void handleSlotWidgetRequestHideDetailed();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    Player* m_player;
    bool m_isHBoxLayout;
    bool m_isLandMark;
    bool m_isSelf;

    QScrollArea* m_scrollArea = nullptr;
    QWidget* m_contentWidget = nullptr;
    QBoxLayout* m_cardLayout = nullptr;
    QList<SlotWidget*> m_slots;
};

#endif // PLAYERAREAWIDGET_H
