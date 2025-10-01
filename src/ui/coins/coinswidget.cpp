#include "coinswidget.h"
#include <QDebug>       // 用于调试，可选
#include <QEasingCurve> // 用于动画缓动效果

CoinsWidget::CoinsWidget(QWidget* parent, int coinsNum)
    : QWidget(parent),
    m_targetCoinsNum(coinsNum), // 初始化目标金币数量
    m_mainLayout(new QStackedLayout(this)),
    m_coinsIconLabel(new QLabel(this)),
    m_textLabel(new QLabel(this)),
    m_animation(new QVariantAnimation(this)), // 初始化动画对象
    m_firstSetDone(false) // 标志位初始化为 false
{
    // 全显示
    m_mainLayout->setStackingMode(QStackedLayout::StackAll);
    // 当前文字居中
    m_coinsIconLabel->setAlignment(Qt::AlignCenter);
    m_textLabel->setAlignment(Qt::AlignCenter);
    m_coinsIconLabel->setText("🪙");

    // 构造函数中直接设置文本，不触发动画。
    // 第一次调用 setCoinsNum 时会处理 m_firstSetDone 逻辑。
    m_textLabel->setText(QString("%1").arg(m_targetCoinsNum));
    // 初始样式为白色
    m_textLabel->setStyleSheet("QLabel { color:white; }");

    m_mainLayout->addWidget(m_textLabel);
    m_mainLayout->addWidget(m_coinsIconLabel);

    setLayout(m_mainLayout);

    // 连接动画的 valueChanged 信号到自定义槽函数
    connect(m_animation, &QVariantAnimation::valueChanged, this, &CoinsWidget::updateAnimatedCoins);
    // 当动画结束时，确保文本显示的是最终目标值并重置颜色
    connect(m_animation, &QVariantAnimation::finished, this, [this]() {
        m_textLabel->setText(QString("%1").arg(m_targetCoinsNum));
        m_textLabel->setStyleSheet("QLabel { color: white; }"); // 动画结束后恢复白色
    });
}

CoinsWidget::~CoinsWidget()
{
    // QObject 的父子关系会处理 m_animation, m_mainLayout, m_coinsIconLabel, m_textLabel 的删除
}

void CoinsWidget::setCoinsNum(int coinsNum)
{
    // 如果是首次设置金币数量（即 m_firstSetDone 为 false），则直接设置值并返回，不播放动画
    if (!m_firstSetDone) {
        m_targetCoinsNum = coinsNum;
        m_textLabel->setText(QString("%1").arg(m_targetCoinsNum));
        m_textLabel->setStyleSheet("QLabel { color: white; }"); // 确保首次设置时是白色
        m_firstSetDone = true; // 标记已完成首次设置
        return; // 首次设置不播放动画
    }

    // 从此处开始，都是后续设置，需要播放动画

    // 获取当前显示的金币数量作为动画的起始值
    int currentDisplayedCoins = m_textLabel->text().toInt();

    // 如果目标金币数量与当前目标相同，且当前显示的数字也相同，则不进行操作
    if (coinsNum == m_targetCoinsNum && coinsNum == currentDisplayedCoins) {
        return;
    }

    // 如果当前有动画正在运行，则停止它，并以其当前值作为新动画的起始值
    if (m_animation->state() == QAbstractAnimation::Running) {
        m_animation->stop();
        currentDisplayedCoins = m_animation->currentValue().toInt();
    }

    // 更新目标金币数量
    m_targetCoinsNum = coinsNum;

    // 根据金币变化方向设置文本颜色
    if (m_targetCoinsNum > currentDisplayedCoins) {
        m_textLabel->setStyleSheet("QLabel { color: #66BB6A; }"); // 增加时显示绿色
    } else if (m_targetCoinsNum < currentDisplayedCoins) {
        m_textLabel->setStyleSheet("QLabel { color: #EF5350; }");   // 减少时显示红色
    } else {
        // 如果目标和当前显示值相同（例如，动画被中断后重新设置为相同的值），
        // 保持当前颜色或恢复白色，这里我们选择恢复白色以避免意外的颜色残留
        m_textLabel->setStyleSheet("QLabel { color: white; }");
    }

    // 设置动画的起始值和结束值
    m_animation->setStartValue(currentDisplayedCoins);
    m_animation->setEndValue(m_targetCoinsNum);
    // 设置动画持续时间（例如，500毫秒）
    m_animation->setDuration(500);
    // 设置缓动曲线，使动画更平滑
    m_animation->setEasingCurve(QEasingCurve::OutQuad);

    // 启动动画
    m_animation->start();
}

void CoinsWidget::updateAnimatedCoins(const QVariant& value)
{
    // 将动画的当前值转换为整数并更新 QLabel 的文本
    m_textLabel->setText(QString("%1").arg(value.toInt()));
}

// 重写 resizeEvent 以自我调整尺寸和宽高比
void CoinsWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    const QSize allocatedSize = event->size();
    int width = allocatedSize.width();
    int height = allocatedSize.height();

    int fontSize = qMax(5, height / 3);
    int iconSize = qMax(5, height / 2);
    m_textLabel->setFont(QFont("ExtraBlack", fontSize, QFont::Black));
    m_coinsIconLabel->setFont(QFont("YouYuan", iconSize, QFont::Bold));
}
