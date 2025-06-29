#ifndef CARDSTOREAREAWIDGET_H
#define CARDSTOREAREAWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QList>
#include <QLabel> // 用于在没有商店时显示提示

#include "gamestate.h" // 包含 CardStore, Card
#include "cardstorewidget.h" // 包含 CardStoreWidget

class CardStoreAreaWidget : public QWidget
{
    Q_OBJECT
public:
    // 新增构造函数：供 Qt Designer/uic 使用，只接受父对象
    explicit CardStoreAreaWidget(QWidget* parent = nullptr); // <--- 新增

    ~CardStoreAreaWidget();

    // 新增方法：在 ui->setupUi() 之后设置 GameState
    void setGameState(GameState* gameState); // <--- 新增

private:
    GameState* m_gameState;
    QVBoxLayout* m_mainLayout;
    QList<CardStoreWidget*> m_storeWidgets; // 存储 CardStoreWidget 实例

    void commonInit(); // 提取公共初始化逻辑
    void initializeStoreWidgets(); // 根据 GameState 初始化商店部件
};

#endif // CARDSTOREAREAWIDGET_H
