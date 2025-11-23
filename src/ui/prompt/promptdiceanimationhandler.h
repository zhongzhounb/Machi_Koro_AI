#ifndef PROMPT_DICE_ANIMATION_HANDLER_H
#define PROMPT_DICE_ANIMATION_HANDLER_H

#include "PromptHandlerBase.h"
#include <QTimer>
#include <QPointer>

class PromptDiceAnimationHandler : public PromptHandlerBase {
    Q_OBJECT
public:
    explicit PromptDiceAnimationHandler(MainWindow* main);

    void handle(const PromptData& pd) override;
};

#endif
