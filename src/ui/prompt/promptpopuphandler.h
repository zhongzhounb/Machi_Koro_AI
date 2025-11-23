#ifndef PROMPT_POPUP_HANDLER_H
#define PROMPT_POPUP_HANDLER_H

#include "PromptHandlerBase.h"
#include <QTimer>
#include <QPointer>

class PromptPopupHandler : public PromptHandlerBase {
    Q_OBJECT
public:
    explicit PromptPopupHandler(MainWindow* main);

    void handle(const PromptData& pd) override;
};

#endif
