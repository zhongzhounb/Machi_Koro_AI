#ifndef CARDSTOREWIDGET_H
#define CARDSTOREWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QList>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QQueue> // 新增：包含 QQueue
#include <QPair>  // 新增：包含 QPair

#include "cardstore.h"
#include "cardwidget.h"

class CardStoreWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CardStoreWidget(CardStore* store, QWidget* parent = nullptr);
    ~CardStoreWidget();

    CardStore* getCardStore() const { return m_store; }

public slots:
    void onStoreInit(CardStore* store, int supplyPileNum, QList<QList<Card*>> slot);
    void onCardAdded(CardStore* store, Card* card, int pos);

private slots: // 新增私有槽
    void startNextAnimation(); // 处理动画队列中的下一个动画

private:
    CardStore* m_store;
    QLabel* m_supplyPileLabel;
    QList<QWidget*> m_cardSlots;
    QHBoxLayout* m_mainLayout;

    QLabel* m_animatedCardLabel; // 动画的临时标签

    QQueue<QPair<Card*, int>> m_animationQueue; // 动画请求队列
    bool m_animationInProgress; // 动画是否正在进行

    void setupUI();
    QWidget* createEmptySlotWidget();
    void setSlotContent(int pos, Card* card);
};

#endif // CARDSTOREWIDGET_H
