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
    enum BackgroundState {
        Dawn,
        Noon,
        Dusk,
        Night,
        DeepNight,
        NumStates
    };
    Q_ENUM(BackgroundState)

    explicit GameBackgroundWidget(QWidget *parent = nullptr);
    ~GameBackgroundWidget();

public slots:
    void advanceState();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    BackgroundState m_currentState;
    int m_animationDurationMs = 2000; // 动画持续时间（毫秒）

    SkyPainter *m_skyPainter;
    CitySilhouettePainter *m_cityPainter;

    QParallelAnimationGroup *m_backgroundAnimationGroup;
    QAnimationGroup *m_sunMoonAnimationGroup;

    QPropertyAnimation *m_skyTopColorAnimation;
    QPropertyAnimation *m_skyBottomColorAnimation;
    QPropertyAnimation *m_cloudColorAnimation;
    QPropertyAnimation *m_starColorAnimation;
    QPropertyAnimation *m_cityColorAnimation;

    QPropertyAnimation *m_cloudMovementAnimation; // 云朵移动动画，现在控制 cloudBaseOffsetX

    struct StateParameters {
        QColor skyTopColor;
        QColor skyBottomColor;
        QColor sunMoonColor;
        QColor cloudColor;
        QColor starColor;
        QPointF sunMoonPosition;
        QColor cityColor;
    };

    StateParameters getParametersForState(BackgroundState state);
    void setInitialState(BackgroundState state);
    QPropertyAnimation* createConfiguredAnimation(QObject* target, const QByteArray& propertyName, int duration, const QEasingCurve& curve, QObject* parent = nullptr);
    void setSunMoonAnimationGroup(QAnimationGroup* newGroup);
};

#endif // GAMEBACKGROUNDWIDGET_H
