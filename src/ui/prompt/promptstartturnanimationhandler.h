#ifndef PROMPT_START_TURN_ANIMATION_HANDLER_H
#define PROMPT_START_TURN_ANIMATION_HANDLER_H

#include "PromptHandlerBase.h"
#include <QTimer>
#include <QLabel>
#include <QGraphicsOpacityEffect>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>
#include <QPauseAnimation>
#include <QPointer>

class PromptStartTurnAnimationHandler : public PromptHandlerBase {
    Q_OBJECT
public:
    explicit PromptStartTurnAnimationHandler(MainWindow* main);

    void handle(const PromptData& pd) override;
};

#endif
