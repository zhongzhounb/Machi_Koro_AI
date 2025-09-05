#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QPointer>
#include <functional>
#include "global.h"
// 前向声明，减少头文件依赖
class GameState;
class Player;
class CardStore;
class Card;
class PlayerAreaWidget;
class CardStoreAreaWidget;
class CardWidget;
class QGridLayout;
class QGraphicsDropShadowEffect;
class QPropertyAnimation;
class PromptData;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// 辅助结构体，用于配置每个玩家的UI布局
struct PlayerLayoutConfig {
    int photoRow, photoCol, photoRowSpan, photoColSpan;
    int cardAreaRow, cardAreaCol, cardAreaRowSpan, cardAreaColSpan;
    bool cardAreaIsHorizontal;

    bool hasLandmarkArea; // 是否有地标区域
    int landmarkAreaRow, landmarkAreaCol, landmarkAreaRowSpan, landmarkAreaColSpan;
    bool landmarkAreaIsHorizontal;
    bool landmarkAreaIsSpecialPlayer0; // 仅用于玩家0的地标区

    // 使用std::function来动态计算窗口外位置，因为m_gameMainWidget的尺寸可能在构造函数时尚未确定
    std::function<QPoint(int gameMainWidgetWidth, int gameMainWidgetHeight)> getOutOfWindowPos;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(GameState* state, QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void responseUserInput(int opId);

public slots:
    void onRequestUserInput(PromptData pd);

private slots:
    void showDetailedCard(Card* card, QPoint globalPos);
    void hideDetailedCard();

private:
    void setupGameMainLayout(QGridLayout* layout, const QList<Player*>& players);
    void setupPlayerWidgets(QGridLayout* layout, Player* player, const PlayerLayoutConfig& config);
    CardStore* findCardStoreForCard(Card* card, int& posInStore);

    Ui::MainWindow *ui;
    GameState* m_state;

    QWidget* m_gameMainWidget;
    QWidget* m_animationOverlayWidget;

    CardStoreAreaWidget* m_cardStoreArea;

    QMap<Player*, PlayerAreaWidget*> m_playerToCardAreaMap;
    QMap<Player*, PlayerAreaWidget*> m_playerToLandmarkAreaMap;

    // ******** 新增：存储每个玩家的布局配置，包括计算窗口外位置的函数 ********
    QMap<Player*, PlayerLayoutConfig> m_playerLayoutConfigs;
    // ******** 结束新增 ********

    QMap<Player*, QPoint> m_playerOutOfWindowTargetPos; // 存储计算好的窗口外目标位置 (此Map将在动画前动态更新)

    // 用于详细卡牌显示和动画
    QPointer<CardWidget> m_detailedCardWidget = nullptr;
    QPointer<QPropertyAnimation> m_fadeAnimation = nullptr;
};

#endif // MAINWINDOW_H
