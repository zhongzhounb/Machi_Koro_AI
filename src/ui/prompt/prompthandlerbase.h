#ifndef PROMPT_HANDLER_BASE_H
#define PROMPT_HANDLER_BASE_H

#include <QObject>
#include "global.h"

class MainWindow;

class PromptHandlerBase : public QObject {
    Q_OBJECT
public:
    explicit PromptHandlerBase(MainWindow* main);

    virtual void handle(const PromptData& pd) = 0;

signals:
    void responseUserInput(int id);

protected:
    MainWindow* m_main;
};

#endif
