#ifndef SLOTWIDGET_H
#define SLOTWIDGET_H
#include <QFrame>
#include <QList>
#include <QStackedLayout>
#include <QLabel>
#include <QString>
#include "global.h"

class CardWidget; // 前向声明
class Card;       // 前向声明 Card

class SlotWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SlotWidget(bool isSupplyPile, Color supplyColor, QWidget* parent = nullptr);
    ~SlotWidget();

    void pushCard(CardWidget* cardWidget);
    CardWidget* topCard() const;
    CardWidget* popCard();
    int cardCount() const { return m_cards.size(); }
    bool isEmpty() const { return m_cards.isEmpty(); }

    void addCount(); //如果是供应堆才有的函数
    void updateCardSize(int cardHeight); // 新增：更新内部所有卡牌尺寸的方法

    QSize sizeHint() const override;

signals:
    // 当槽位中的顶层卡牌被点击时发出信号，不再传递槽位索引，由父级确定
    void topCardClickedInSlot(Card* clickedCard);


private slots:
    void onTopCardClicked(Card* card); // 监听内部 CardWidget 的点击信号，并转发

private:
    //是否为供应堆
    bool m_isSupplyPile;
    //1.是，则记录供应堆类型
    Color m_supplyColor;
    //2.不是，则记录卡槽卡牌
    QList<CardWidget*> m_cards;
    //1&2.卡牌数量
    int m_displayedCount;


    QLabel* m_countOverlayLabel;//数量
    QStackedLayout* m_stackedLayout;//卡槽（堆叠布局）

    CardWidget* m_currentTopCardWidget = nullptr; // 指向当前顶层显示的 CardWidget（最好不用）

    void initUI();
    void updateDisplay();
    void updatePosition();

    QRect scaledRect(const QRect& originalRect, qreal scaleX, qreal scaleY);
};

#endif // SLOTWIDGET_H
