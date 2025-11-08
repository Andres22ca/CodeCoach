
#ifndef GUI_CODECOACH_COACHFEEDBACKWIDGET_H
#define GUI_CODECOACH_COACHFEEDBACKWIDGET_H

#pragma once
#include <QWidget>
#include "../viewmodels/dto/CoachFeedback.h"

class QLabel;
class QListWidget;
class QPlainTextEdit;

class CoachFeedbackWidget : public QWidget {
    Q_OBJECT
public:
    explicit CoachFeedbackWidget(QWidget* parent=nullptr);

public slots:
    void render(const cc::dto::CoachFeedback& fb);
    void addHint(const QString& h); // opcional incremental

private:
    QLabel* headline_ = nullptr;
    QListWidget* hints_ = nullptr;
    QPlainTextEdit* reasoning_ = nullptr;
};



#endif //GUI_CODECOACH_COACHFEEDBACKWIDGET_H //para mostrar hints