#ifndef PLAYERAREAWIDGET_H
#define PLAYERAREAWIDGET_H

#include <QWidget>
#include <QList>
#include <QMap>
#include <QBoxLayout> // 新增：包含 QBoxLayout 头文件

// 前向声明
class Player;
class Card;
class SlotWidget;
class CardWidget;
// class QHBoxLayout; // 不再需要单独前向声明，因为 QBoxLayout 已包含
// class QVBoxLayout; // 不再需要单独前向声明
class QScrollArea;

enum class State;

class PlayerAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerAreaWidget(Player* player, bool isHBoxLayout, bool isLandMark, QWidget* parent = nullptr, bool isSelf = false);
    ~PlayerAreaWidget();

    QPoint getCardTargetSlotCenterPos(Card* card);
    QSize getTargetCardSize();

private slots:
    void onCardAdded(Player* player, Card* card);
    void onCardDeled(Player* player, Card* card);
    void onCardStateChanged(Card* card, State state);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    Player* m_player;
    bool m_isHBoxLayout;
    bool m_isLandMark;
    bool m_isSelf;

    QScrollArea* m_scrollArea = nullptr;
    QWidget* m_contentWidget = nullptr;
    QBoxLayout* m_cardLayout = nullptr;    // <--- 这里修改为 QBoxLayout*
    QList<SlotWidget*> m_slots;
};

#endif // PLAYERAREAWIDGET_H
