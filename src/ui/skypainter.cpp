#include "SkyPainter.h"
#include <QWidget>
#include <QDebug>

SkyPainter::SkyPainter(QObject *parent)
    : QObject(parent)
    , m_currentState(GameBackgroundWidget::Dawn)
{
    initializeCloudData(); // 在构造函数中初始化云朵数据
}

// Setter 实现，并发出信号，请求父Widget重绘
void SkyPainter::setSkyTopColor(const QColor& color) {
    if (m_skyTopColor != color) {
        m_skyTopColor = color;
        emit skyTopColorChanged();
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}
void SkyPainter::setSkyBottomColor(const QColor& color) {
    if (m_skyBottomColor != color) {
        m_skyBottomColor = color;
        emit skyBottomColorChanged();
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}
void SkyPainter::setSunMoonColor(const QColor& color) {
    if (m_sunMoonColor != color) {
        m_sunMoonColor = color;
        emit sunMoonColorChanged();
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}
void SkyPainter::setCloudColor(const QColor& color) {
    if (m_cloudColor != color) {
        m_cloudColor = color;
        emit cloudColorChanged();
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}
void SkyPainter::setStarColor(const QColor& color) {
    if (m_starColor != color) {
        m_starColor = color;
        emit starColorChanged();
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}
void SkyPainter::setSunMoonPosition(const QPointF& pos) {
    if (m_sunMoonPosition != pos) {
        m_sunMoonPosition = pos;
        emit sunMoonPositionChanged();
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}
void SkyPainter::setCloudBaseOffsetX(qreal offset) {
    if (!qFuzzyCompare(m_cloudBaseOffsetX, offset)) {
        m_cloudBaseOffsetX = offset;
        emit cloudBaseOffsetXChanged();
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}

void SkyPainter::setSize(const QSize& size)
{
    if (m_size != size) {
        m_size = size;
        // 尺寸改变后，重新初始化云朵数据，确保初始偏移量基于新尺寸
        initializeCloudData();
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}

void SkyPainter::setBackgroundState(GameBackgroundWidget::BackgroundState state)
{
    m_currentState = state;
}

void SkyPainter::paint(QPainter* painter)
{
    if (m_size.isEmpty()) return;

    QLinearGradient skyGradient(0, 0, 0, m_size.height());
    skyGradient.setColorAt(0, m_skyTopColor);
    skyGradient.setColorAt(1, m_skyBottomColor);
    painter->fillRect(0, 0, m_size.width(), m_size.height(), skyGradient);

    drawSunMoon(painter);

    if (m_currentState == GameBackgroundWidget::Night || m_currentState == GameBackgroundWidget::DeepNight) {
        drawStars(painter);
    }

    if (m_currentState == GameBackgroundWidget::Dawn || m_currentState == GameBackgroundWidget::Noon || m_currentState == GameBackgroundWidget::Dusk) {
        drawClouds(painter);
    }
}

void SkyPainter::drawSunMoon(QPainter* painter)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_sunMoonColor);

    qreal sunMoonRadius = qMin(m_size.width(), m_size.height()) * 0.08;

    qreal moonX = m_sunMoonPosition.x();
    qreal moonY = m_sunMoonPosition.y();

    QPainterPath fullMoonPath;
    fullMoonPath.addEllipse(moonX - sunMoonRadius, moonY - sunMoonRadius, sunMoonRadius * 2, sunMoonRadius * 2);

    if (m_currentState == GameBackgroundWidget::DeepNight) {
        QPainterPath subtractPath;
        qreal offset = sunMoonRadius * 0.4;
        subtractPath.addEllipse(moonX - sunMoonRadius + offset, moonY - sunMoonRadius, sunMoonRadius * 2, sunMoonRadius * 2);
        painter->fillPath(fullMoonPath.subtracted(subtractPath), m_sunMoonColor);
    } else {
        painter->fillPath(fullMoonPath, m_sunMoonColor);
    }
}

void SkyPainter::drawStars(QPainter* painter)
{
    if (m_starColor.alpha() == 0) return;

    painter->setPen(Qt::NoPen);
    painter->setBrush(m_starColor);

    int numStars = (m_currentState == GameBackgroundWidget::DeepNight) ? 60 : 30;
    qreal starSize = qMin(m_size.width(), m_size.height()) * 0.005;

    for (int i = 0; i < numStars; ++i) {
        qreal x = RandomUtils::instance().generateInt(0, m_size.width() - 1);
        qreal y = RandomUtils::instance().generateInt(0, m_size.height() * 0.7 - 1);

        painter->drawEllipse(x - starSize / 2, y - starSize / 2, starSize, starSize);
    }
}

// 初始化云朵数据
void SkyPainter::initializeCloudData() {
    // 定义云朵形状模板
    QVector<QVector<CloudPart>> cloudTemplates = {
        { // Cloud Group 1 Template (左侧)
            {0.1, 0.2, 0.15, 0.08},
            {0.15, 0.18, 0.12, 0.07},
            {0.08, 0.19, 0.1, 0.06}
        },
        { // Cloud Group 2 Template (中间偏左)
            {0.35, 0.1, 0.18, 0.09},
            {0.43, 0.08, 0.14, 0.08},
            {0.30, 0.11, 0.1, 0.07}
        },
        { // Cloud Group 3 Template (中间偏右)
            {0.6, 0.15, 0.2, 0.1},
            {0.68, 0.12, 0.15, 0.09},
            {0.55, 0.16, 0.1, 0.07}
        },
        { // Cloud Group 4 Template (右侧) - 新增，增加密度
            {0.85, 0.22, 0.16, 0.08},
            {0.90, 0.20, 0.13, 0.07},
            {0.80, 0.23, 0.11, 0.06}
        }
    };

    m_cloudData.clear();
    // 为每个模板生成一个速度因子和初始偏移，并存储模板
    for (const auto& templateParts : cloudTemplates) {
        CloudData data;
        data.speedFactor = RandomUtils::instance().generateInt(80, 120) / 100.0; // 速度因子在 0.8 到 1.2 之间随机
        // 初始随机偏移，让云朵在动画开始时就分散开
        // 如果 m_size.width() 为 0，给一个默认宽度，避免除零或生成负数
        data.initialOffsetX = RandomUtils::instance().generateInt(0, m_size.width() > 0 ? m_size.width() : 800);
        data.templateParts = templateParts; // 存储模板
        m_cloudData.append(data);
    }
}

void SkyPainter::drawClouds(QPainter* painter)
{
    if (m_cloudColor.alpha() == 0 || m_size.isEmpty()) return;

    painter->setPen(Qt::NoPen); // 修正：NoNoPen 不是有效的枚举值
    painter->setBrush(m_cloudColor);

    // The animation for cloudBaseOffsetX goes from 0 to m_size.width()
    // We want clouds to move from right to left.
    // So, as m_cloudBaseOffsetX increases, clouds should shift left.

    for (const auto& data : m_cloudData) {
        // 计算此云朵组的实际偏移量（向左移动）
        // data.initialOffsetX 是一个随机起始点，m_cloudBaseOffsetX * data.speedFactor 是动画导致的移动量
        qreal effectiveGroupOffset = data.initialOffsetX - (m_cloudBaseOffsetX * data.speedFactor);

        // 将偏移量包装到 [0, m_size.width()) 范围内，表示模式的起始点
        qreal wrappedOffset = fmod(effectiveGroupOffset, m_size.width());
        if (wrappedOffset < 0) {
            wrappedOffset += m_size.width(); // 确保结果为正
        }

        // 绘制两套云朵以实现无缝循环。
        // 第一套云朵
        for (const auto& part : data.templateParts) {
            qreal x = part.relX * m_size.width() + wrappedOffset;
            qreal y = part.relY * m_size.height();
            qreal w = part.relW * m_size.width();
            qreal h = part.relH * m_size.height();
            painter->drawEllipse(x, y, w, h);
        }

        // 第二套云朵，在第一套的右侧，用于在第一套移出屏幕时无缝衔接
        for (const auto& part : data.templateParts) {
            qreal x = part.relX * m_size.width() + wrappedOffset + m_size.width();
            qreal y = part.relY * m_size.height();
            qreal w = part.relW * m_size.width();
            qreal h = part.relH * m_size.height();
            painter->drawEllipse(x, y, w, h);
        }
    }
}
