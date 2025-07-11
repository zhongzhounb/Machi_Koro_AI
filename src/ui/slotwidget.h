#ifndef SLOTWIDGET_H
#define SLOTWIDGET_H

#include <QFrame>
#include <QList>
#include <QStackedLayout>
#include <QLabel>
#include "global.h"

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
    void addCount();

signals:
    void topCardClickedInSlot(Card* clickedCard);

private slots:
    void onTopCardClicked(Card* card);

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
