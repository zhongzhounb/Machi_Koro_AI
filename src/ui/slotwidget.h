#ifndef SLOTWIDGET_H
#define SLOTWIDGET_H
#include <QFrame>
#include <QList>
#include <QStackedLayout>
#include <QLabel>
#include <QString>

class CardWidget; // 前向声明
class Card;       // 前向声明 Card

class SlotWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SlotWidget(bool isSupplyPile, const QString& supplyPileBackImagePath = "", QWidget* parent = nullptr);
    ~SlotWidget();

    void pushCard(CardWidget* cardWidget);
    CardWidget* topCard() const;
    CardWidget* popCard();
    int cardCount() const { return m_cards.size(); }
    bool isEmpty() const { return m_cards.isEmpty(); }

    void setDisplayedCount(int count); // 新增：用于供应堆数量更新

signals:
    // 当槽位中的顶层卡牌被点击时发出信号，不再传递槽位索引，由父级确定
    void topCardClickedInSlot(Card* clickedCard);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onTopCardClicked(Card* card); // 监听内部 CardWidget 的点击信号，并转发

private:
    bool m_isSupplyPile;
    QLabel* m_backgroundLabel;
    QLabel* m_countOverlayLabel;
    QList<CardWidget*> m_cards;
    QStackedLayout* m_stackedLayout;
    QString m_supplyPileBackImagePath;

    CardWidget* m_currentTopCardWidget = nullptr; // 指向当前顶层显示的 CardWidget
    int m_displayedCount = -1; // -1 表示使用 m_cards.size()，否则用于供应堆覆盖显示数量

    void initUI();
    void updateDisplay();
    void updatePosition();

    const int SLOT_REF_WIDTH = 100;
    const int SLOT_REF_HEIGHT = 150;
    const QRect IMG_DISPLAY_RECT = QRect(0, 0, SLOT_REF_WIDTH, SLOT_REF_HEIGHT);

    const QString EMPTY_SLOT_IMAGE_PATH = ":/resources/images/card/slot.png";

    QRect scaledRect(const QRect& originalRect, qreal scaleX, qreal scaleY);
};

#endif // SLOTWIDGET_H
