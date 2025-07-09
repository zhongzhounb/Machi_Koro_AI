#ifndef PLAYERAREAWIDGET_H
#define PLAYERAREAWIDGET_H
#include <QWidget>
#include <QGridLayout> // 改变为 QGridLayout
#include <QList>
#include <QLabel>
#include <QMap> // 用于存储 CardStore 到 SlotWidget 列表的映射
#include <QQueue> // 用于动画队列
#include <QPropertyAnimation> // 用于动画


class SlotWidget; // 前向声明
class Card;       // 前向声明
class CardWidget; // 前向声明
class CardStore;  // 前向声明
class Player;

class PlayerAreaWidget:public QWidget
{
    Q_OBJECT
public:
    explicit PlayerAreaWidget(Player* player,bool isHBoxLayout,bool isLandMark,QWidget* parent = nullptr);
    ~PlayerAreaWidget();

public slots:
    void onCardAdded(Player* player,Card* card);

private:
    Player* m_player;
    bool m_isHBoxLayout;
    bool m_isLandMark;
    QBoxLayout* m_mainLayout;
    QList<SlotWidget*> m_slots;
};

#endif // PLAYERAREAWIDGET_H
