#include "logviewerwidget.h"

LogViewerWidget::LogViewerWidget(QWidget* parent) : QWidget(parent) {
    // 创建 QTextEdit
    m_logTextEdit = new QTextEdit(this);
    m_logTextEdit->setReadOnly(true); // 设置为只读
    m_logTextEdit->setLineWrapMode(QTextEdit::WidgetWidth); // 自动换行

    // 使用 QVBoxLayout 将 QTextEdit 放入 Widget
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_logTextEdit);
    setLayout(layout);
    setStyleSheet("border: 1px solid #ccc; background-color: #f8f8f8;");
}

LogViewerWidget::~LogViewerWidget() {
}

void LogViewerWidget::appendLogMessage(const QString& message) {

    m_logTextEdit->append(message); // 将新消息添加到 QTextEdit 底部
    // 自动滚动到底部
    m_logTextEdit->verticalScrollBar()->setValue(m_logTextEdit->verticalScrollBar()->maximum());
}
