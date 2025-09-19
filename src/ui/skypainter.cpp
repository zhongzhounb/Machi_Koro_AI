#include "SkyPainter.h"
#include <QWidget>
#include <QDebug>
// #include <QRadialGradient> // 移除径向渐变头文件，不再使用

SkyPainter::SkyPainter(QObject *parent)
    : QObject(parent)
    , m_currentState(GameBackgroundWidget::Dawn)
{
    initializeCloudData();
    // generateStarPositions 会在 setSize 被调用时初始化
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

// 新增：卡牌区域 setter
void SkyPainter::setCardRect(const QRectF& rect)
{
    if (m_cardRectRel != rect) {
        m_cardRectRel = rect;
        generateStarPositions(); // 卡牌区域改变时重新生成星星位置
        emit cardRectChanged();
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}

void SkyPainter::setSize(const QSize& size)
{
    if (m_size != size) {
        m_size = size;
        initializeCloudData(); // 尺寸改变后重新初始化云朵数据
        generateStarPositions(); // 尺寸改变后重新生成星星位置
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}

void SkyPainter::setBackgroundState(GameBackgroundWidget::BackgroundState state)
{
    if (m_currentState != state) { // 避免不必要的更新
        m_currentState = state;
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}

void SkyPainter::paint(QPainter* painter)
{
    if (m_size.isEmpty()) return;

    // 绘制天空渐变背景
    bool isDaytime = (m_currentState == GameBackgroundWidget::Dawn || m_currentState == GameBackgroundWidget::Noon || m_currentState == GameBackgroundWidget::Dusk);

    QLinearGradient skyGradient(0, 0, 0, m_size.height()); // 总是垂直线性渐变

    if (m_currentState == GameBackgroundWidget::Dawn) {
        // 清晨：三层渐变：顶部蓝色，中间黄色，底部橙色
        // 注意：线性渐变 setColorAt(0) 是顶部，setColorAt(1) 是底部
        skyGradient.setColorAt(0, m_skyTopColor);    // 顶部蓝色
        skyGradient.setColorAt(0.5*0.9, QColor("#faedde"));
        skyGradient.setColorAt(0.75*0.9, QColor("#fdd7a8"));
        skyGradient.setColorAt(0.9*0.9, QColor("#ffa688"));
        skyGradient.setColorAt(1, m_skyBottomColor); // 底部橙色
    } else if (m_currentState == GameBackgroundWidget::Noon) {
        // 正午：太阳在顶部，所以天空顶部更亮 (m_skyBottomColor)，底部更暗 (m_skyTopColor)
        skyGradient.setColorAt(0, m_skyTopColor); // 顶部更亮
        skyGradient.setColorAt(1, m_skyBottomColor);    // 底部更暗
    } else if (m_currentState == GameBackgroundWidget::Dusk) {
        // 黄昏：太阳在底部，所以天空顶部更暗 (m_skyTopColor)，底部更亮 (m_skyBottomColor)
        skyGradient.setColorAt(0, m_skyTopColor);    // 顶部更暗
        skyGradient.setColorAt(0.5*0.9, QColor("#faedde"));
        skyGradient.setColorAt(0.75*0.9, QColor("#fdd7a8"));
        skyGradient.setColorAt(0.9*0.9, QColor("#ffa688"));
        skyGradient.setColorAt(1, m_skyBottomColor); // 底部更亮
    } else { // 夜晚
        // 夜晚：保持顶部暗，底部亮 (月亮在上方，所以顶部暗，底部亮合理)
        skyGradient.setColorAt(0, m_skyTopColor);
        skyGradient.setColorAt(1, m_skyBottomColor);
    }
    painter->fillRect(0, 0, m_size.width(), m_size.height(), skyGradient);

    drawSunMoon(painter);

    // 只有在 Night 状态下绘制星星
    if (m_currentState == GameBackgroundWidget::Night) {
        drawStars(painter);
    }

    if (isDaytime) { // 只有白天状态才绘制云朵
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

    // 月亮总是满月
    painter->fillPath(fullMoonPath, m_sunMoonColor);
}

// 新增：生成星星位置的方法
void SkyPainter::generateStarPositions()
{
    m_starPositions.clear(); // 清除旧的星星位置
    if (m_size.isEmpty()) return;

    int numStars = 45; // 固定数量的星星
    qreal starSize = qMin(m_size.width(), m_size.height()) * 0.005;

    // 将相对卡牌区域转换为绝对像素坐标
    QRectF absoluteCardRect(m_cardRectRel.x() * m_size.width(),
                            m_cardRectRel.y() * m_size.height(),
                            m_cardRectRel.width() * m_size.width(),
                            m_cardRectRel.height() * m_size.height());

    for (int i = 0; i < numStars; ++i) {
        QPointF starPos;
        bool validPositionFound = false;
        int attempts = 0;
        const int maxAttempts = 100; // 防止无限循环，如果有效区域太小

        while (!validPositionFound && attempts < maxAttempts) {
            qreal x = RandomUtils::instance().generateInt(0, m_size.width() - 1);
            qreal y = RandomUtils::instance().generateInt(0, m_size.height() * 0.7 - 1); // 星星只在上部70%的天空生成
            starPos = QPointF(x, y);

            // 检查星星的包围盒是否与卡牌区域重叠
            QRectF starBoundingBox(x - starSize / 2, y - starSize / 2, starSize, starSize);

            if (!absoluteCardRect.intersects(starBoundingBox)) { // 如果不重叠，则位置有效
                validPositionFound = true;
            }
            attempts++;
        }

        if (validPositionFound) {
            m_starPositions.append(starPos);
        } else {
            qWarning() << "Could not find a valid position for a star after" << maxAttempts << "attempts.";
        }
    }
}

void SkyPainter::drawStars(QPainter* painter)
{
    if (m_starColor.alpha() == 0 || m_starPositions.isEmpty()) return;

    painter->setPen(Qt::NoPen);
    painter->setBrush(m_starColor);

    qreal starSize = qMin(m_size.width(), m_size.height()) * 0.005;

    // 绘制已存储的星星位置
    for (const QPointF& starPos : m_starPositions) {
        painter->drawEllipse(starPos.x() - starSize / 2, starPos.y() - starSize / 2, starSize, starSize);
    }
}

void SkyPainter::initializeCloudData() {
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
        { // Cloud Group 4 Template (右侧)
            {0.85, 0.22, 0.16, 0.08},
            {0.90, 0.20, 0.13, 0.07},
            {0.80, 0.23, 0.11, 0.06}
        }
    };

    m_cloudData.clear();
    for (const auto& templateParts : cloudTemplates) {
        CloudData data;
        data.speedFactor = 0.0; // 将速度因子设置为0，使云朵静止
        data.initialOffsetX = RandomUtils::instance().generateInt(0, m_size.width() > 0 ? m_size.width() : 800); // 初始随机偏移
        data.templateParts = templateParts; // 云朵形状模板
        m_cloudData.append(data); // 添加到云朵数据列表
    }
}

void SkyPainter::drawClouds(QPainter* painter)
{
    if (m_cloudColor.alpha() == 0 || m_size.isEmpty()) return;

    painter->setPen(Qt::NoPen);
    painter->setBrush(m_cloudColor);

    for (const auto& data : m_cloudData) {
        // 对于静止的云朵，effectiveGroupOffset 只需要考虑 initialOffsetX
        qreal effectiveGroupOffset = data.initialOffsetX;

        // 由于云朵是静止的，不需要处理 wrappedOffset 和绘制第二组云朵
        for (const auto& part : data.templateParts) {
            qreal x = part.relX * m_size.width() + effectiveGroupOffset;
            qreal y = part.relY * m_size.height();
            qreal w = part.relW * m_size.width();
            qreal h = part.relH * m_size.height();
            painter->drawEllipse(x, y, w, h);
        }
    }
}
