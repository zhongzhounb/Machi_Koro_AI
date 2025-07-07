#include "AutoFitTextLabel.h"
#include <QResizeEvent>
#include <QFont>

AutoFitTextLabel::AutoFitTextLabel(QWidget *parent)
    : QLabel(parent)
{
    // 禁用自动换行，确保文本只占一行
    setWordWrap(false);
    // 设置文本居中，根据需要调整
    setAlignment(Qt::AlignCenter);
}

AutoFitTextLabel::AutoFitTextLabel(const QString &text, QWidget *parent)
    : QLabel(text, parent)
{
    setWordWrap(false);
    setAlignment(Qt::AlignCenter);
    m_currentText = text; // 存储初始文本
    updateTextFont(); // 初始设置字体
}

void AutoFitTextLabel::setText(const QString &text)
{
    QLabel::setText(text); // 调用基类的setText
    m_currentText = text;  // 更新内部存储的文本
    updateTextFont();      // 重新计算字体大小
}

void AutoFitTextLabel::resizeEvent(QResizeEvent *event)
{
    QLabel::resizeEvent(event); // 调用基类的resizeEvent
    updateTextFont();           // 重新计算字体大小
}

void AutoFitTextLabel::updateTextFont()
{
    if (m_currentText.isEmpty()) {
        // 如果文本为空，设置一个默认字体大小，或者清空字体
        QFont font = this->font();
        font.setPointSize(10); // 默认字体大小
        setFont(font);
        return;
    }

    // 获取QLabel的可用宽度（减去边距）
    int availableWidth = contentsRect().width();
    if (availableWidth <= 0) {
        return; // 宽度无效，不处理
    }

    // 使用二分查找法找到最大的合适字体大小
    int minFontSize = 1;
    int maxFontSize = 200; // 设置一个合理的上限，避免无限循环或过大字体

    int bestFitFontSize = minFontSize;

    while (minFontSize <= maxFontSize) {
        int currentFontSize = (minFontSize + maxFontSize) / 2;
        if (currentFontSize == 0) { // 避免除零或无限循环
            currentFontSize = 1;
        }

        QFont font = this->font(); // 获取当前字体对象
        font.setPointSize(currentFontSize); // 设置测试字体大小

        QFontMetrics metrics(font); // 创建字体度量对象

        // 计算文本的像素宽度
        int textWidth = metrics.horizontalAdvance(m_currentText);

        if (textWidth <= availableWidth) {
            // 如果文本宽度小于等于可用宽度，说明当前字体大小可行，尝试更大的字体
            bestFitFontSize = currentFontSize;
            minFontSize = currentFontSize + 1;
        } else {
            // 如果文本宽度大于可用宽度，说明当前字体太大，尝试更小的字体
            maxFontSize = currentFontSize - 1;
        }
    }

    // 应用找到的最佳字体大小
    QFont finalFont = this->font();
    finalFont.setPointSize(bestFitFontSize);
    setFont(finalFont);
}
