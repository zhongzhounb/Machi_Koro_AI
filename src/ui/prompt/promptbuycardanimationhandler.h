#ifndef PROMPT_BUY_CARD_ANIMATION_HANDLER_H
#define PROMPT_BUY_CARD_ANIMATION_HANDLER_H

#include "PromptHandlerBase.h"
#include <QTimer>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>
#include <QPauseAnimation>
#include <QGraphicsDropShadowEffect>

class PromptBuyCardAnimationHandler : public PromptHandlerBase {
    Q_OBJECT
public:
    explicit PromptBuyCardAnimationHandler(MainWindow* main);

    void handle(const PromptData& pd) override;
};

#endif
