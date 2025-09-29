#ifndef GAMEBACKGROUNDWIDGET_H
#define GAMEBACKGROUNDWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QDateTime>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>

// 前向声明
class SkyPainter;
class CitySilhouettePainter;

class GameBackgroundWidget : public QWidget
{
    Q_OBJECT

public:
    // 定义背景状态枚举 (移除了 DeepNight 状态)
    enum BackgroundState {
        Dawn,        // 清晨
        Noon,        // 正午
        Dusk,        // 黄昏
        Night,       // 夜晚
        NumStates    // 状态数量，用于循环 (现在是 4)
    };
    Q_ENUM(BackgroundState)

    explicit GameBackgroundWidget(QWidget *parent = nullptr);
    ~GameBackgroundWidget();

public slots:
    // 槽函数：接收信号以切换到下一个日夜状态
    void advanceState();

protected:
    // 绘制事件，所有绘图操作都在此进行
    void paintEvent(QPaintEvent *event) override;
    // 尺寸改变事件，用于更新子绘图器的尺寸
    void resizeEvent(QResizeEvent *event) override;

private:
    BackgroundState m_currentState; // 当前的背景状态
    int m_animationDurationMs = 2000; // 动画持续时间（毫秒），略微增加以适应复杂路径

    SkyPainter *m_skyPainter;             // 天空绘图器
    CitySilhouettePainter *m_cityPainter; // 城市剪影绘图器

    // 背景颜色动画组 (总是并行运行)
    QParallelAnimationGroup *m_backgroundAnimationGroup;

    // 太阳/月亮动画组 (可以是并行或顺序，根据状态动态创建/替换)
    QAnimationGroup *m_sunMoonAnimationGroup; // 使用基类指针，方便替换

    // 各个属性的动画对象 (这些是具体的QPropertyAnimation实例，作为GameBackgroundWidget的子对象)
    QPropertyAnimation *m_skyTopColorAnimation;
    QPropertyAnimation *m_skyBottomColorAnimation;
    QPropertyAnimation *m_cloudColorAnimation;
    QPropertyAnimation *m_starColorAnimation;
    QPropertyAnimation *m_cityColorAnimation;

    // 云朵移动动画 (已移除，云朵将静止)
    // QPropertyAnimation *m_cloudMovementAnimation;

    // 结构体，用于存储某个状态的所有绘图参数
    struct StateParameters {
        QColor skyTopColor;
        QColor skyBottomColor;
        QColor sunMoonColor;
        QColor cloudColor;
        QColor starColor;
        QPointF sunMoonPosition;
        QColor cityColor;
    };

    // 根据状态获取对应的参数
    StateParameters getParametersForState(BackgroundState state);

    // 设置初始状态（无动画）
    void setInitialState(BackgroundState state);

    // 辅助函数：创建并配置一个QPropertyAnimation，允许指定父对象
    QPropertyAnimation* createConfiguredAnimation(QObject* target, const QByteArray& propertyName, int duration, const QEasingCurve& curve, QObject* parent = nullptr);

    // 辅助函数：清理旧的太阳/月亮动画组并设置新的
    void setSunMoonAnimationGroup(QAnimationGroup* newGroup);

    // 辅助函数：设置窗户的配置
    void setupWindows();
};

#endif // GAMEBACKGROUNDWIDGET_H
