#include "cardstoreareawidget.h"
#include "slotwidget.h"
#include "card.h"
#include "cardwidget.h"
#include "cardstore.h"
#include <QDebug>
#include <QLabel>
#include <QTimer> // QTimer 不再需要，但可能用于其他地方
// #include <QResizeEvent> // 不再需要

CardStoreAreaWidget::CardStoreAreaWidget(QWidget* parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_mainLayout = new QGridLayout(this);
    setLayout(m_mainLayout);
}

CardStoreAreaWidget::~CardStoreAreaWidget() {}

void CardStoreAreaWidget::setGameState(GameState* gameState) {
    if (m_gameState == gameState) return;
    m_gameState = gameState;
    initializeStoreWidgets();
}

void CardStoreAreaWidget::initializeStoreWidgets()
{
    QLayoutItem *item;
    while ((item = m_mainLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    m_storeToSlotsMap.clear();

    if (!m_gameState) return;
    QList<CardStore*> stores = m_gameState->getCardStores();
    if (stores.isEmpty()) return;

    int maxSlotsInAnyStore = 0;
    for (CardStore* store : stores) {
        maxSlotsInAnyStore = qMax(maxSlotsInAnyStore, store->getStoreSlotsCount() + 1);
    }

    int row_index = 0;
    for (CardStore* store : stores) {
        QList<SlotWidget*> currentStoreSlots;

        SlotWidget* supplySlot = new SlotWidget(true, store->getType(), this);
        m_mainLayout->addWidget(supplySlot, row_index, 0);
        currentStoreSlots.append(supplySlot);
        connect(store, &CardStore::supplyCardAdded, this, &CardStoreAreaWidget::onSupplyCardAdded);

        for (int i = 0; i < store->getStoreSlotsCount(); i++) {
            SlotWidget* slot = new SlotWidget(false, Color::BackNone, this);
            QList<Card*> cards = store->getSlots().at(i);
            for (Card* card : cards) {
                CardWidget* cardWidget = new CardWidget(card, ShowType::Ordinary, slot);
                slot->pushCard(cardWidget);
            }
            m_mainLayout->addWidget(slot, row_index, i + 1);
            currentStoreSlots.append(slot);
        }

        m_storeToSlotsMap.insert(store, currentStoreSlots);
        connect(store, &CardStore::cardAdded, this, &CardStoreAreaWidget::onCardAdded);
        row_index++;
    }

    for(int i = 0; i < maxSlotsInAnyStore; i++)
        m_mainLayout->setColumnStretch(i, 1);
    for(int i = 0; i < stores.size(); i++)
        m_mainLayout->setRowStretch(i, 1);


}

void CardStoreAreaWidget::onCardAdded(CardStore* store, Card* card, int pos)
{
    if (!m_storeToSlotsMap.contains(store)) return;

    pos++; // 位移，因为m_slots[0]是供应堆
    QList<SlotWidget*> storeSlots = m_storeToSlotsMap.value(store);
    if (pos >= storeSlots.size()) return;

    storeSlots[0]->popCard();
    CardWidget* newCardWidget = new CardWidget(card, ShowType::Ordinary, storeSlots[pos]);
    storeSlots[pos]->pushCard(newCardWidget);

}

void CardStoreAreaWidget::onSupplyCardAdded(CardStore* store){
    if (!m_storeToSlotsMap.contains(store)) return;
    m_storeToSlotsMap.value(store).at(0)->addCount();
}

