#include "cardstoreareawidget.h"
#include <QDebug>
#include <QLabel> // 确保包含 QLabel

// 提取公共初始化逻辑
void CardStoreAreaWidget::commonInit() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    m_mainLayout->setSpacing(20); // 商店行之间的间距
    m_gameState = nullptr; // 初始时将 GameState 设置为 nullptr
}

// 新增构造函数：供 Qt Designer/uic 使用
CardStoreAreaWidget::CardStoreAreaWidget(QWidget* parent)
    : QWidget(parent)
{
    commonInit(); // 调用公共初始化
    // GameState 将通过 setGameState() 方法稍后设置
}

CardStoreAreaWidget::~CardStoreAreaWidget()
{
    // 子部件已设置为当前部件的父对象，会在当前部件析构时自动删除。
}

// 新增方法：设置 GameState 并初始化商店部件
void CardStoreAreaWidget::setGameState(GameState* gameState) {
    if (m_gameState == gameState) return; // 避免重复设置
    m_gameState = gameState;
    initializeStoreWidgets(); // 现在 GameState 已设置，可以初始化 UI 了
}

void CardStoreAreaWidget::initializeStoreWidgets()
{
    if (!m_gameState) {
        qWarning() << "CardStoreAreaWidget: GameState is null. Cannot initialize store widgets.";
        // 清除现有布局内容，并显示一个提示
        QLayoutItem *item;
        while ((item = m_mainLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        QLabel* noStateLabel = new QLabel("No GameState provided yet.", this);
        noStateLabel->setAlignment(Qt::AlignCenter);
        m_mainLayout->addWidget(noStateLabel);
        m_mainLayout->addStretch();
        return;
    }

    // 清除任何现有的部件和布局项
    qDeleteAll(m_storeWidgets);
    m_storeWidgets.clear();

    QLayoutItem *item;
    while ((item = m_mainLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater(); // 安全删除部件
        }
        delete item; // 删除布局项
    }

    QList<CardStore*> stores = m_gameState->getCardStores();
    if (stores.isEmpty()) {
        qDebug() << "No card stores found in GameState.";
        QLabel* noStoresLabel = new QLabel("No card stores available.", this);
        noStoresLabel->setAlignment(Qt::AlignCenter);
        m_mainLayout->addWidget(noStoresLabel);
        m_mainLayout->addStretch();
        return;
    }

    for (CardStore* store : stores) {
        CardStoreWidget* storeWidget = new CardStoreWidget(store, this); // 父对象设置为当前部件
        m_storeWidgets.append(storeWidget);
        m_mainLayout->addWidget(storeWidget);
    }
    m_mainLayout->addStretch(); // 将内容推到顶部
}
