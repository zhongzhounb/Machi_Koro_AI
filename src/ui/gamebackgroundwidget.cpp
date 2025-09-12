#include "GameBackgroundWidget.h"
#include "SkyPainter.h"
#include "CitySilhouettePainter.h"

#include <QPainter>
#include <QDebug>
#include <QEasingCurve>
#include <QObject>

QPropertyAnimation* GameBackgroundWidget::createConfiguredAnimation(QObject* target, const QByteArray& propertyName, int duration, const QEasingCurve& curve, QObject* parent)
{
    QPropertyAnimation* anim = new QPropertyAnimation(target, propertyName, parent);
    anim->setDuration(duration);
    anim->setEasingCurve(curve);
    return anim;
}

void GameBackgroundWidget::setSunMoonAnimationGroup(QAnimationGroup* newGroup)
{
    if (m_sunMoonAnimationGroup) {
        m_sunMoonAnimationGroup->stop();
        m_sunMoonAnimationGroup->deleteLater();
    }
    m_sunMoonAnimationGroup = newGroup;
    if (m_sunMoonAnimationGroup && m_sunMoonAnimationGroup->parent() != this) {
        m_sunMoonAnimationGroup->setParent(this);
    }
}


GameBackgroundWidget::GameBackgroundWidget(QWidget *parent)
    : QWidget(parent)
    , m_currentState(Dawn)
    , m_backgroundAnimationGroup(nullptr)
    , m_sunMoonAnimationGroup(nullptr)
{
    m_skyPainter = new SkyPainter(this);
    m_cityPainter = new CitySilhouettePainter(this);

    m_backgroundAnimationGroup = new QParallelAnimationGroup(this);

    m_skyTopColorAnimation = createConfiguredAnimation(m_skyPainter, "skyTopColor", m_animationDurationMs, QEasingCurve::InOutQuad, this);
    m_skyBottomColorAnimation = createConfiguredAnimation(m_skyPainter, "skyBottomColor", m_animationDurationMs, QEasingCurve::InOutQuad, this);
    m_cloudColorAnimation = createConfiguredAnimation(m_skyPainter, "cloudColor", m_animationDurationMs, QEasingCurve::InOutQuad, this);
    m_starColorAnimation = createConfiguredAnimation(m_skyPainter, "starColor", m_animationDurationMs, QEasingCurve::InOutQuad, this);
    m_cityColorAnimation = createConfiguredAnimation(m_cityPainter, "cityColor", m_animationDurationMs, QEasingCurve::InOutQuad, this);

    m_backgroundAnimationGroup->addAnimation(m_skyTopColorAnimation);
    m_backgroundAnimationGroup->addAnimation(m_skyBottomColorAnimation);
    m_backgroundAnimationGroup->addAnimation(m_cloudColorAnimation);
    m_backgroundAnimationGroup->addAnimation(m_starColorAnimation);
    m_backgroundAnimationGroup->addAnimation(m_cityColorAnimation);

    // 新增：初始化云朵移动动画
    // 动画时长增加10倍，实现0.1倍速效果
    m_cloudMovementAnimation = createConfiguredAnimation(m_skyPainter, "cloudBaseOffsetX", m_animationDurationMs * 10, QEasingCurve::Linear, this);
    m_cloudMovementAnimation->setLoopCount(-1); // 无限循环

    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setInitialState(m_currentState);
}

GameBackgroundWidget::~GameBackgroundWidget()
{
    // QObject的父子关系会自动删除子对象。
}

void GameBackgroundWidget::setInitialState(BackgroundState state)
{
    StateParameters params = getParametersForState(state);

    m_skyPainter->setSkyTopColor(params.skyTopColor);
    m_skyPainter->setSkyBottomColor(params.skyBottomColor);
    m_skyPainter->setSunMoonColor(params.sunMoonColor);
    m_skyPainter->setCloudColor(params.cloudColor);
    m_skyPainter->setStarColor(params.starColor);
    m_skyPainter->setSunMoonPosition(params.sunMoonPosition);
    m_skyPainter->setBackgroundState(state);
    m_cityPainter->setCityColor(params.cityColor);

    // 新增：设置云朵基准偏移为0
    m_skyPainter->setCloudBaseOffsetX(0);

    bool isDaytime = (state == Dawn || state == Noon || state == Dusk);
    if (isDaytime) {
        if (m_cloudMovementAnimation->state() != QAbstractAnimation::Running) {
            // 设置云朵动画的起始和结束值，使其从 0 移动到屏幕宽度，然后循环
            // 动画结束后，cloudBaseOffsetX 会重置为 startValue (0)，通过 drawClouds 的双层绘制实现无缝
            m_cloudMovementAnimation->setStartValue(0.0);
            m_cloudMovementAnimation->setEndValue(this->width());
            m_cloudMovementAnimation->start();
        }
    } else {
        if (m_cloudMovementAnimation->state() == QAbstractAnimation::Running) {
            m_cloudMovementAnimation->stop();
            m_skyPainter->setCloudBaseOffsetX(0); // 停止时将云朵位置重置
        }
    }

    update();
}

void GameBackgroundWidget::advanceState()
{
    if (m_backgroundAnimationGroup->state() == QAbstractAnimation::Running) {
        m_backgroundAnimationGroup->stop();
    }
    if (m_sunMoonAnimationGroup && m_sunMoonAnimationGroup->state() == QAbstractAnimation::Running) {
        m_sunMoonAnimationGroup->stop();
    }

    BackgroundState nextState = static_cast<BackgroundState>((m_currentState + 1) % NumStates);
    qDebug() << "Switching from state:" << m_currentState << "to state:" << nextState;

    StateParameters targetParams = getParametersForState(nextState);

    m_skyTopColorAnimation->setStartValue(m_skyPainter->skyTopColor());
    m_skyTopColorAnimation->setEndValue(targetParams.skyTopColor);

    m_skyBottomColorAnimation->setStartValue(m_skyPainter->skyBottomColor());
    m_skyBottomColorAnimation->setEndValue(targetParams.skyBottomColor);

    m_cloudColorAnimation->setStartValue(m_skyPainter->cloudColor());
    m_cloudColorAnimation->setEndValue(targetParams.cloudColor);

    m_starColorAnimation->setStartValue(m_skyPainter->starColor());
    m_starColorAnimation->setEndValue(targetParams.starColor);

    m_cityColorAnimation->setStartValue(m_cityPainter->cityColor());
    m_cityColorAnimation->setEndValue(targetParams.cityColor);

    if ((m_currentState == Dusk && nextState == Night) || (m_currentState == DeepNight && nextState == Dawn)) {
        QSequentialAnimationGroup *sunMoonSeqGroup = new QSequentialAnimationGroup(this);

        QPropertyAnimation *posAnim1 = createConfiguredAnimation(m_skyPainter, "sunMoonPosition", m_animationDurationMs / 2, QEasingCurve::InQuad, sunMoonSeqGroup);
        QPropertyAnimation *colorAnim1 = createConfiguredAnimation(m_skyPainter, "sunMoonColor", m_animationDurationMs / 2, QEasingCurve::InQuad, sunMoonSeqGroup);

        posAnim1->setStartValue(m_skyPainter->sunMoonPosition());
        colorAnim1->setStartValue(m_skyPainter->sunMoonColor());

        if (m_currentState == Dusk) {
            posAnim1->setEndValue(QPointF(width() * 1.2, height() * 1.2));
        } else {
            posAnim1->setEndValue(QPointF(width() * -0.2, height() * 1.2));
        }
        colorAnim1->setEndValue(QColor(0,0,0,0));

        QParallelAnimationGroup *phase1Group = new QParallelAnimationGroup(sunMoonSeqGroup);
        phase1Group->addAnimation(posAnim1);
        phase1Group->addAnimation(colorAnim1);
        sunMoonSeqGroup->addAnimation(phase1Group);

        QPropertyAnimation *posAnim2 = createConfiguredAnimation(m_skyPainter, "sunMoonPosition", m_animationDurationMs / 2, QEasingCurve::OutQuad, sunMoonSeqGroup);
        QPropertyAnimation *colorAnim2 = createConfiguredAnimation(m_skyPainter, "sunMoonColor", m_animationDurationMs / 2, QEasingCurve::OutQuad, sunMoonSeqGroup);

        if (nextState == Night) {
            posAnim2->setStartValue(QPointF(width() * -0.2, height() * 1.2));
        } else {
            posAnim2->setStartValue(QPointF(width() * 1.2, height() * 1.2));
        }
        posAnim2->setEndValue(targetParams.sunMoonPosition);

        colorAnim2->setStartValue(QColor(0,0,0,0));
        colorAnim2->setEndValue(targetParams.sunMoonColor);

        QParallelAnimationGroup *phase2Group = new QParallelAnimationGroup(sunMoonSeqGroup);
        phase2Group->addAnimation(posAnim2);
        phase2Group->addAnimation(colorAnim2);
        sunMoonSeqGroup->addAnimation(phase2Group);

        setSunMoonAnimationGroup(sunMoonSeqGroup);

    } else {
        QParallelAnimationGroup* directSunMoonGroup = new QParallelAnimationGroup(this);

        QPropertyAnimation *posAnim = createConfiguredAnimation(m_skyPainter, "sunMoonPosition", m_animationDurationMs, QEasingCurve::InOutQuad, directSunMoonGroup);
        QPropertyAnimation *colorAnim = createConfiguredAnimation(m_skyPainter, "sunMoonColor", m_animationDurationMs, QEasingCurve::InOutQuad, directSunMoonGroup);

        posAnim->setStartValue(m_skyPainter->sunMoonPosition());
        posAnim->setEndValue(targetParams.sunMoonPosition);

        colorAnim->setStartValue(m_skyPainter->sunMoonColor());
        colorAnim->setEndValue(targetParams.sunMoonColor);

        directSunMoonGroup->addAnimation(posAnim);
        directSunMoonGroup->addAnimation(colorAnim);

        setSunMoonAnimationGroup(directSunMoonGroup);
    }

    // --- 云朵移动动画的启动/停止逻辑 ---
    bool isDaytime = (nextState == Dawn || nextState == Noon || nextState == Dusk);
    if (isDaytime) {
        if (m_cloudMovementAnimation->state() != QAbstractAnimation::Running) {
            // 每次启动时，确保动画从当前位置开始，并以屏幕宽度为终点
            m_cloudMovementAnimation->setStartValue(m_skyPainter->cloudBaseOffsetX());
            m_cloudMovementAnimation->setEndValue(this->width());
            m_cloudMovementAnimation->start();
        }
    } else {
        if (m_cloudMovementAnimation->state() == QAbstractAnimation::Running) {
            m_cloudMovementAnimation->stop();
            m_skyPainter->setCloudBaseOffsetX(0); // 停止时将云朵位置重置
        }
    }

    m_skyPainter->setBackgroundState(nextState);

    m_backgroundAnimationGroup->start();
    if (m_sunMoonAnimationGroup) {
        m_sunMoonAnimationGroup->start();
    }

    m_currentState = nextState;
}

void GameBackgroundWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    m_skyPainter->paint(&painter);
    m_cityPainter->paint(&painter);
}

void GameBackgroundWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    m_skyPainter->setSize(size());
    m_cityPainter->setSize(size());

    if (m_backgroundAnimationGroup->state() == QAbstractAnimation::Running) {
        m_backgroundAnimationGroup->stop();
    }
    if (m_sunMoonAnimationGroup && m_sunMoonAnimationGroup->state() == QAbstractAnimation::Running) {
        m_sunMoonAnimationGroup->stop();
    }
    if (m_cloudMovementAnimation->state() == QAbstractAnimation::Running) { // 停止云朵动画
        m_cloudMovementAnimation->stop();
    }

    setInitialState(m_currentState); // 重新设置初始状态以适应新尺寸，这也会重新启动或停止云朵动画

    update();
}

GameBackgroundWidget::StateParameters GameBackgroundWidget::getParametersForState(BackgroundState state)
{
    StateParameters params;
    qreal width = this->width();
    qreal height = this->height();

    switch (state) {
    case Dawn:
        params.skyTopColor = QColor("#FFD1DC");
        params.skyBottomColor = QColor("#ADD8E6");
        params.sunMoonColor = QColor("#FFEB3B");
        params.cloudColor = QColor("#FFFFFF");
        params.starColor = QColor(0,0,0,0);
        params.sunMoonPosition = QPointF(width * 0.1, height * 0.6);
        params.cityColor = QColor("#800080");
        break;
    case Noon:
        params.skyTopColor = QColor("#87CEEB");
        params.skyBottomColor = QColor("#B0E0E6");
        params.sunMoonColor = QColor("#FFEB3B");
        params.cloudColor = QColor("#FFFFFF");
        params.starColor = QColor(0,0,0,0);
        params.sunMoonPosition = QPointF(width * 0.5, height * 0.15);
        params.cityColor = QColor("#4682B4");
        break;
    case Dusk:
        params.skyTopColor = QColor("#FF6347");
        params.skyBottomColor = QColor("#DA70D6");
        params.sunMoonColor = QColor("#FF4500");
        params.cloudColor = QColor("#FFFAF0");
        params.starColor = QColor(0,0,0,0);
        params.sunMoonPosition = QPointF(width * 0.9, height * 0.6);
        params.cityColor = QColor("#4B0082");
        break;
    case Night:
        params.skyTopColor = QColor("#191970");
        params.skyBottomColor = QColor("#483D8B");
        params.sunMoonColor = QColor("#F5F5DC");
        params.cloudColor = QColor(0,0,0,0);
        params.starColor = QColor("#FFFFFF");
        params.sunMoonPosition = QPointF(width * 0.8, height * 0.2);
        params.cityColor = QColor("#000000");
        break;
    case DeepNight:
        params.skyTopColor = QColor("#000033");
        params.skyBottomColor = QColor("#000066");
        params.sunMoonColor = QColor("#DCDCDC");
        params.cloudColor = QColor(0,0,0,0);
        params.starColor = QColor("#F0F8FF");
        params.sunMoonPosition = QPointF(width * 0.8, height * 0.2);
        params.cityColor = QColor("#000000");
        break;
    default:
        break;
    }
    return params;
}
