#ifndef LOGVIEWERWIDGET_H
#define LOGVIEWERWIDGET_H

#include <QWidget>
#include <QTextEdit> // 用于显示文本
#include <QVBoxLayout> // 用于布局
#include <QScrollBar> // 用于自动滚动

class LogViewerWidget : public QWidget {
    Q_OBJECT
public:
    explicit LogViewerWidget(QWidget* parent = nullptr);
    ~LogViewerWidget();

public slots:
    // 接收日志消息的槽函数
    void appendLogMessage(const QString& message);

private:
    QTextEdit* m_logTextEdit;
};

#endif // LOGVIEWERWIDGET_H
