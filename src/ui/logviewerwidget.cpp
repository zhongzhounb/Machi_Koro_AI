#include "logviewerwidget.h"
#include <QPainter>
#include <QFontMetrics>
#include <QScrollBar>

// ====================== LogContentWidget ==========================
LogContentWidget::LogContentWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    m_colors = {"#FFB347", "#87CEEB", "#FF69B4", "#9370DB","#CD5C5C","#000000"};
    m_font = QFont("YouYuan", 11,QFont::Bold);
}

void LogContentWidget::addMessage(const QString& message)
{
    if (message.contains("的回合")) {
        if(message.contains("艾米"))
            m_colorIndex = 0;
        else if(message.contains("汐月"))
            m_colorIndex = 1;
        else if(message.contains("莉莉"))
            m_colorIndex = 2;
        else if(message.contains("星辰"))
            m_colorIndex = 3;
        else if(message.contains("琉璃"))
            m_colorIndex = 4;
        else
            m_colorIndex = 5;

        m_blocks.append({message, QColor(m_colors[m_colorIndex])});
    } else {
        if (m_blocks.isEmpty())
            m_blocks.append({QString(), QColor(m_colors[5])});
        m_blocks.last().text += (m_blocks.last().text.isEmpty() ? "" : "\n") + message;
    }

    updateGeometry();
    update();
}

void LogContentWidget::clearLogs()
{
    m_blocks.clear();
    updateGeometry();
    update();
}

QSize LogContentWidget::sizeHint() const
{
    QFontMetrics fm(m_font);
    int totalHeight = 10;
    int widthAvailable = width() > 0 ? width() - 20 : 380; // 预估宽度

    for (const auto& block : m_blocks) {
        QStringList lines = block.text.split('\n', Qt::SkipEmptyParts);
        if (lines.isEmpty()) continue;

        int blockHeight = 0;
        for (const QString& line : lines) {
            QRect textRect = fm.boundingRect(0, 0, widthAvailable - 20, 10000,
                                             Qt::TextWordWrap, line);
            blockHeight += textRect.height() + 4;
        }
        totalHeight += blockHeight + 20; // 每个块间距
    }
    return QSize(400, totalHeight);
}

void LogContentWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setFont(m_font);

    int y = 10;
    int xPadding = 10;
    int widthAvailable = width() - xPadding * 2;
    QFontMetrics fm(m_font);

    QTextOption textOpt;
    textOpt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

    for (const auto& block : m_blocks) {
        QStringList lines = block.text.split('\n', Qt::SkipEmptyParts);
        if (lines.isEmpty()) continue;

        // 计算整个块高度
        int blockHeight = 0;
        QList<QRect> rects;
        for (const QString& line : lines) {
            QRect textRect = fm.boundingRect(0, 0, widthAvailable - 20, 10000,
                                             Qt::TextWordWrap, line);
            rects.append(textRect);
            blockHeight += textRect.height() + 4;
        }

        QRect rect(xPadding, y, widthAvailable, blockHeight + 10);

        // 绘制圆角矩形背景
        QColor semiColor = block.color;
        semiColor.setAlpha(160);  // 0~255，数值越小越透明，可调，比如 120~180 较合适
        p.setBrush(semiColor);
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(rect, 10, 10);

        // 绘制文字
        p.setPen(QColor("#222222"));
        int textY = y + 8;
        for (int i = 0; i < lines.size(); ++i) {
            QRect textRect(xPadding + 10, textY, widthAvailable - 20, rects[i].height());
            p.drawText(textRect, Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignTop, lines[i], &textRect);
            textY += rects[i].height() + 4;
        }

        y += blockHeight + 20;
    }
}

// ====================== LogViewerWidget ==========================

LogViewerWidget::LogViewerWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
    setStyleSheet("background: transparent; border: none;");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_content = new LogContentWidget(this);
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidget(m_content);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setStyleSheet("background: transparent; border: none;");
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    layout->addWidget(m_scrollArea);
    setLayout(layout);

    // —— 这里添加滚动条圆角样式 ——
    const char* roundedScrollbarQss = R"(
    QScrollBar:vertical {
        background: transparent; width: 10px; margin: 6px 2px 6px 0; border-radius: 5px;
    }
    QScrollBar::handle:vertical {
        background: rgba(0,0,0,90); min-height: 36px; border-radius: 5px;
    }
    QScrollBar::handle:vertical:hover   { background: rgba(0,0,0,120); }
    QScrollBar::handle:vertical:pressed { background: rgba(0,0,0,160); }
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; border: none; background: transparent; }
    QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }

    QScrollBar:horizontal {
        background: transparent; height: 10px; margin: 0 6px 2px 6px; border-radius: 5px;
    }
    QScrollBar::handle:horizontal {
        background: rgba(0,0,0,90); min-width: 36px; border-radius: 5px;
    }
    QScrollBar::handle:horizontal:hover   { background: rgba(0,0,0,120); }
    QScrollBar::handle:horizontal:pressed { background: rgba(0,0,0,160); }
    QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; border: none; background: transparent; }
    QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal { background: transparent; }
    )";

    m_scrollArea->verticalScrollBar()->setStyleSheet(roundedScrollbarQss);
    m_scrollArea->horizontalScrollBar()->setStyleSheet(roundedScrollbarQss);
}

void LogViewerWidget::appendLogMessage(const QString& message)
{
    m_content->addMessage(message);

    // 自动滚动到底部
    QScrollBar* bar = m_scrollArea->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void LogViewerWidget::clearLogs()
{
    m_content->clearLogs();
}
