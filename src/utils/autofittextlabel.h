#ifndef AUTOFITTEXTLABEL_H
#define AUTOFITTEXTLABEL_H

#include <QLabel>
#include <QFontMetrics> // 用于测量文本尺寸

class AutoFitTextLabel : public QLabel
{
    Q_OBJECT
public:
    explicit AutoFitTextLabel(QWidget *parent = nullptr);
    explicit AutoFitTextLabel(const QString &text, QWidget *parent = nullptr);

    // 重写setText，以便在文本改变时重新计算字体大小
    void setText(const QString &text);

protected:
    // 重写resizeEvent，以便在尺寸改变时重新计算字体大小
    void resizeEvent(QResizeEvent *event) override;

private:
    // 核心函数：根据当前尺寸和文本计算最佳字体大小
    void updateTextFont();

    QString m_currentText; // 存储当前文本，因为QLabel::text()可能不是最新的
};

#endif // AUTOFITTEXTLABEL_H
