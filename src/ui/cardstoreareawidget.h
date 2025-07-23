#ifndef CARDSTOREAREAWIDGET_H
#define CARDSTOREAREAWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QList>
#include <QMap>
#include "gamestate.h"

class SlotWidget;
class Card;
class CardWidget;
class CardStore;

class CardStoreAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CardStoreAreaWidget(QWidget* parent = nullptr);
    ~CardStoreAreaWidget();

    void setGameState(GameState* gameState);

private slots:
    void onSupplyCardAdded(CardStore* store);
    void onCardAdded(CardStore* store, Card* card, int pos);
    void onCardDeled(CardStore* store,Card* card,int pos);

private:
    GameState* m_gameState = nullptr;
    QGridLayout* m_mainLayout = nullptr;
    QMap<CardStore*, QList<SlotWidget*>> m_storeToSlotsMap;

    void initializeStoreWidgets();
    // !!! 移除 updateAllCardSizes 函数声明 !!!
};

#endif // CARDSTOREAREAWIDGET_H
