// #ifndef CARDSTOREWIDGET_H
// #define CARDSTOREWIDGET_H

// #include <QWidget>
// #include <QLabel>
// #include <QHBoxLayout>
// #include <QList>
// #include <QPropertyAnimation>
// #include <QGraphicsOpacityEffect>
// #include <QQueue>
// #include <QPair>

// #include "cardstore.h"
// #include "cardwidget.h" // <--- 确保包含 CardWidget

// class CardStoreWidget : public QWidget
// {
//     Q_OBJECT
// public:
//     explicit CardStoreWidget(CardStore* store, QWidget* parent = nullptr);
//     ~CardStoreWidget();

//     CardStore* getCardStore() const { return m_store; }

// public slots:
//     void onStoreInit(CardStore* store, int supplyPileNum, QList<QList<Card*>> slot);
//     void onCardAdded(CardStore* store, Card* card, int pos);

// private slots:
//     void startNextAnimation();

// private:
//     CardStore* m_store;
//     QHBoxLayout* m_mainLayout; // 主布局

//     QList<QWidget*> m_displaySlots; // 统一的显示槽位列表，m_displaySlots[0] 是供应堆

//     CardWidget* m_animatedCardWidget; // <--- 关键修改：直接动画 CardWidget 实例

//     QQueue<QPair<Card*, int>> m_animationQueue;
//     bool m_animationInProgress;

//     void setupUI();
//     void initializeDisplaySlots();
//     QWidget* createSupplyPileWidget();
//     QWidget* createEmptyCardSlotWidget();
//     void updateSlotDisplay(int displayPos, Card* card, int count); // 泛化：更新任意显示槽位的内容
// };

// #endif // CARDSTOREWIDGET_H
