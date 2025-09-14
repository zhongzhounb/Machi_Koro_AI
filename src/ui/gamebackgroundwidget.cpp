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

    m_cloudMovementAnimation = createConfiguredAnimation(m_skyPainter, "cloudBaseOffsetX", m_animationDurationMs * 10, QEasingCurve::Linear, this);
    m_cloudMovementAnimation->setLoopCount(-1);

    setAttribute(Qt::WA_OpaquePaintEvent, true);

    setupWindows(); // 设置窗户

    setInitialState(m_currentState);
}

GameBackgroundWidget::~GameBackgroundWidget()
{
    // QObject的父子关系会自动删除子对象。
}

void GameBackgroundWidget::setupWindows()
{
    m_cityPainter->clearWindows();

    qreal windowW = 0.025; // 单个窗户的相对宽度
    qreal windowH = 0.04;  // 单个窗户的相对高度
    bool litNight = true;  // 假设所有窗户在夜晚都会发光
    bool litDeepNight = true; // 假设所有窗户在深夜都会发光
    qreal groundLevelRel = 0.95; // 假设窗户最低能到达的相对地面高度

    // 辅助函数：在一个建筑块内添加网格状窗户
    // blockXRel: 建筑块的相对X起始坐标
    // blockYTopRel: 建筑块的相对Y顶部坐标
    // blockWidthRel: 建筑块的相对宽度
    // cols: 每行窗户数量
    // rows: 每列窗户数量
    auto addWindowGrid = [&](qreal blockXRel, qreal blockYTopRel, qreal blockWidthRel, int cols, int rows) {
        qreal availableHeightRel = groundLevelRel - blockYTopRel; // 窗户可用的相对高度
        qreal paddingX = (blockWidthRel - cols * windowW) / (cols + 1); // 窗户之间的水平间距
        qreal paddingY = (availableHeightRel - rows * windowH) / (rows + 1); // 窗户之间的垂直间距

        // 确保间距不为负，避免窗户超出建筑块
        if (paddingX < 0) paddingX = 0;
        if (paddingY < 0) paddingY = 0;

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                qreal x = blockXRel + paddingX * (c + 1) + windowW * c;
                qreal y = blockYTopRel + paddingY * (r + 1) + windowH * r;
                m_cityPainter->addWindow(QRectF(x, y, windowW, windowH), litNight, litDeepNight);
            }
        }
    };

    /*
    // 根据 generateBuildingFeaturesAndMainPath() 中定义的建筑块来放置窗户
    // 建筑1 (0.0 到 0.20, 顶部在 0.70)
    addWindowGrid(0.0, 0.70, 0.20, 4, 3);

    // 建筑2 (0.20 到 0.40, 顶部在 0.60) - 三角形屋顶
    addWindowGrid(0.20, 0.60, 0.20, 4, 4);

    // 建筑3 (0.40 到 0.60, 顶部在 0.70) - 左斜坡屋顶
    addWindowGrid(0.40, 0.70, 0.20, 4, 3);

    // 建筑4 (0.60 到 0.80, 顶部在 0.55) - 右斜坡屋顶
    addWindowGrid(0.60, 0.55, 0.20, 4, 5);

    // 建筑5 (0.80 到 1.0, 顶部在 0.65) - 平顶带避雷针
    addWindowGrid(0.80, 0.65, 0.20, 4, 4);*/
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
    m_cityPainter->setBackgroundState(state);

    m_skyPainter->setCloudBaseOffsetX(0);

    bool isDaytime = (state == Dawn || state == Noon || state == Dusk);
    if (isDaytime) {
        if (m_cloudMovementAnimation->state() != QAbstractAnimation::Running) {
            m_cloudMovementAnimation->setStartValue(0.0);
            m_cloudMovementAnimation->setEndValue(this->width());
            m_cloudMovementAnimation->start();
        }
    } else {
        if (m_cloudMovementAnimation->state() == QAbstractAnimation::Running) {
            m_cloudMovementAnimation->stop();
            m_skyPainter->setCloudBaseOffsetX(0);
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

    m_cityPainter->setBackgroundState(nextState);

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

    bool isDaytime = (nextState == Dawn || nextState == Noon || nextState == Dusk);
    if (isDaytime) {
        if (m_cloudMovementAnimation->state() != QAbstractAnimation::Running) {
            m_cloudMovementAnimation->setStartValue(m_skyPainter->cloudBaseOffsetX());
            m_cloudMovementAnimation->setEndValue(this->width());
            m_cloudMovementAnimation->start();
        }
    } else {
        if (m_cloudMovementAnimation->state() == QAbstractAnimation::Running) {
            m_cloudMovementAnimation->stop();
            m_skyPainter->setCloudBaseOffsetX(0);
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
    if (m_cloudMovementAnimation->state() == QAbstractAnimation::Running) {
        m_cloudMovementAnimation->stop();
    }

    setInitialState(m_currentState); // 重新设置初始状态以适应新尺寸

    update();
}

GameBackgroundWidget::StateParameters GameBackgroundWidget::getParametersForState(BackgroundState state)
{
    StateParameters params;
    qreal width = this->width();
    qreal height = this->height();

    switch (state) {
    case Dawn:
        params.skyTopColor = QColor("#9bd7fc");
        params.skyBottomColor = QColor("#ffa688");
        params.sunMoonColor = QColor("#fea48c");
        params.cloudColor = QColor("#FFFFFF");
        params.starColor = QColor(0,0,0,0);
        params.sunMoonPosition = QPointF(width * 0.1, height * 0.7);
        params.cityColor = QColor("#4A2C2A"); // 白天城市统一颜色
        break;
    case Noon:
        params.skyTopColor = QColor("#6495ED"); // 鲜明的矢车菊蓝，晴朗的蓝天
        params.skyBottomColor = QColor("#87CEEB"); // 天蓝色，下方天空
        params.sunMoonColor = QColor("#FFEB3B"); // 鲜亮的黄色，高挂的烈日
        params.cloudColor = QColor("#FFFFFF"); // 洁白的云朵
        params.starColor = QColor(0,0,0,0); // 星星不可见
        params.sunMoonPosition = QPointF(width * 0.5, height * 0.15); // 太阳在天空中央高处
        params.cityColor = QColor("#4A2C2A"); // 白天城市统一颜色
        break;
    case Dusk:
        params.skyTopColor = QColor("#9bd7fc"); // 石板蓝，逐渐向夜晚的深蓝过渡
        params.skyBottomColor = QColor("#ffa688"); // 珊瑚色，温暖而浓郁的落日余晖
        params.sunMoonColor = QColor("#fea48c"); // 橙红色，下沉的夕阳
        params.cloudColor = QColor("#FFB6C1"); // 浅粉色，被夕阳染色的云朵
        params.starColor = QColor(0,0,0,0); // 星星即将出现但仍不可见
        params.sunMoonPosition = QPointF(width * 0.9, height * 0.6); // 太阳在低位
        params.cityColor = QColor("#4A2C2A"); // 白天城市统一颜色
        break;
    case Night:
        params.skyTopColor = QColor("#191970"); // 午夜蓝 (保持不变)
        params.skyBottomColor = QColor("#483D8B"); // 暗板岩蓝 (保持不变)
        params.sunMoonColor = QColor("#F5F5DC"); // 米色 (月亮) (保持不变)
        params.cloudColor = QColor(0,0,0,0); // 夜晚无云，透明 (保持不变)
        params.starColor = QColor("#FFFFFF"); // 白色星星 (保持不变)
        params.sunMoonPosition = QPointF(width * 0.8, height * 0.2); // 月亮位置 (保持不变)
        params.cityColor = QColor("#000000"); // 夜晚房屋为黑色 (保持不变)
        break;
    case DeepNight:
        params.skyTopColor = QColor("#000033"); // 极深蓝 (保持不变)
        params.skyBottomColor = QColor("#000066"); // 更深蓝 (保持不变)
        params.sunMoonColor = QColor("#DCDCDC"); // 浅灰色 (月亮) (保持不变)
        params.cloudColor = QColor(0,0,0,0); // 深夜无云，透明 (保持不变)
        params.starColor = QColor("#F0F8FF"); // 艾利斯蓝 (更多星星) (保持不变)
        params.sunMoonPosition = QPointF(width * 0.8, height * 0.2); // 月亮位置 (保持不变)
        params.cityColor = QColor("#000000"); // 深夜房屋为黑色 (保持不变)
        break;
    default:
        break;
    }
    return params;
}
