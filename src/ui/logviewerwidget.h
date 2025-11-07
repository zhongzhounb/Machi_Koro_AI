#pragma once
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QList>
#include <QColor>
#include <QFont>

struct LogBlock {
    QString text;
    QColor color;
};

class LogContentWidget : public QWidget {
    Q_OBJECT
public:
    explicit LogContentWidget(QWidget* parent = nullptr);
    void addMessage(const QString& message);
    void clearLogs();
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QList<LogBlock> m_blocks;
    QStringList m_colors;
    int m_colorIndex = 0;
    QFont m_font;
};

class LogViewerWidget : public QWidget {
    Q_OBJECT
public:
    explicit LogViewerWidget(QWidget* parent = nullptr);
    void appendLogMessage(const QString& message);
    void clearLogs();

private:
    QScrollArea* m_scrollArea;
    LogContentWidget* m_content;
};
