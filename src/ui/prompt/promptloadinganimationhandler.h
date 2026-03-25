#ifndef PROMPTLOADINGANIMATIONHANDLER_H
#define PROMPTLOADINGANIMATIONHANDLER_H

#include "PromptHandlerBase.h"
#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QGraphicsOpacityEffect>

class PromptLoadingAnimationHandler : public PromptHandlerBase {
    Q_OBJECT
public:
    explicit PromptLoadingAnimationHandler(MainWindow* main);
    void handle(const PromptData& pd) override;

private:
    void showBlackCurtain();
    void startFadeOutAnimation();

    QWidget* m_container = nullptr;
    QLabel* m_loadingLabel = nullptr;
    QGraphicsOpacityEffect* m_opacityEffect = nullptr;
    QTimer* m_dotTimer = nullptr;

    int m_opId = 0;
    int m_dotCount = 0;
    bool m_isClosing = false;
};

#endif
