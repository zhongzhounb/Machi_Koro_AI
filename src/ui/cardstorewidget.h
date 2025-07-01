#ifndef CARDSTOREWIDGET_H
#define CARDSTOREWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QList>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect> // 用于透明度动画，如果需要
#include <QQueue>
#include <QPair>
#include <QDebug> // For debugging

class CardStore;
class SlotWidget;
class Card;
class CardWidget;

// 结构体，用于存储动画所需的数据
struct CardAnimationData {
    Card* cardData;                 // 正在移动的卡牌数据 (用于识别)
    int targetSlotPos;          // 目标槽位的位置 (这里是 CardStore 的 0-indexed 商店槽位索引)
    QPropertyAnimation* animation; // 动画对象
    CardWidget* animatedCardWidget;  // 用于动画的临时 CardWidget
    bool isRemovalAnimation; // 标记是否是移除动画
};

class CardStoreWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CardStoreWidget(CardStore* store, QWidget* parent = nullptr);
    ~CardStoreWidget();

public slots:
    // 槽函数，响应 CardStore 发出的卡牌添加信号
    void onCardAdded(CardStore* store, Card* card, int pos);
    // 槽函数，响应 CardStore 发出的卡牌移除信号 (例如购买)
    //void onCardRemoved(CardStore* store, Card* card, int pos);
    // 槽函数，响应 CardStore 供应堆数量变化
    //void onSupplyCountChanged(CardStore* store, int newCount);

private slots:
    void startNextAnimation(); // 启动队列中的下一个动画
    void animationFinished();  // 动画完成时调用的槽函数

    // 监听 SlotWidget 发出的点击信号
    void onSlotTopCardClicked(Card* clickedCard); // SlotWidget 不再传递索引，由 CardStoreWidget 确定

private:
    CardStore* m_store;
    QHBoxLayout* m_mainLayout; // 主布局
    QList<SlotWidget*> m_slots;// 卡槽列表，m_slots[0] 是供应堆，m_slots[1+] 是商店槽位

    QQueue<CardAnimationData> m_animationQueue; // 动画队列，处理并发添加
    bool m_animationInProgress; // 标记当前是否有动画正在进行

    void initUI();    // 初始化

    // 辅助函数：根据 Card 对象获取其图片路径 (如果需要，但动画直接使用 CardWidget)
    QString getCardImagePath(Card* card);

    // 常量
    const QString SUPPLY_PILE_BACK_IMAGE_PATH = ":/resources/images/card/card_back.png";
};

#endif // CARDSTOREWIDGET_H
