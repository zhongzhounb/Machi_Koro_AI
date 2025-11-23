#ifndef PROMPT_SELECT_DICE_HANDLER_H
#define PROMPT_SELECT_DICE_HANDLER_H

#include "PromptHandlerBase.h"
#include <QTimer>
#include <QPointer>

class PromptSelectDiceHandler : public PromptHandlerBase {
    Q_OBJECT
public:
    explicit PromptSelectDiceHandler(MainWindow* main);

    void handle(const PromptData& pd) override;
};

#endif
