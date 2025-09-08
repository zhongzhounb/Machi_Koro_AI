#ifndef SLOTWIDGET_H
#define SLOTWIDGET_H

#include <QFrame>
#include <QList>
#include <QStackedLayout>
#include <QLabel>
#include "global.h"
#include <QPoint> // 新增：QPoint 头文件

class CardWidget;
class Card;

class SlotWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SlotWidget(bool isSupplyPile, Color supplyColor, QWidget* parent = nullptr);
    ~SlotWidget();

    void pushCard(CardWidget* cardWidget);
    CardWidget* topCard() const;
    CardWidget* popCard();
    bool isEmpty(){return m_displayedCount==0;}
    void delCount();
    void addCount();


signals:
    void topCardClickedInSlot(Card* clickedCard);
    // 新增：转发 CardWidget 的请求显示详细卡牌信号
    void cardWidgetRequestShowDetailed(Card* card, QPoint globalPos);
    // 新增：转发 CardWidget 的请求隐藏详细卡牌信号
    void cardWidgetRequestHideDetailed();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onTopCardClicked(Card* card);
    // 新增：处理内部 CardWidget 的请求
    void handleCardWidgetRequestShowDetailed(Card* card, QPoint globalPos);
    void handleCardWidgetRequestHideDetailed();

private:
    bool m_isSupplyPile;
    Color m_supplyColor;
    QList<CardWidget*> m_cards;
    int m_displayedCount;

    QLabel* m_countOverlayLabel;
    QStackedLayout* m_stackedLayout;
    CardWidget* m_currentTopCardWidget = nullptr;

    void initUI();
    void updateDisplay();

};

#endif // SLOTWIDGET_H
