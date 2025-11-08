//
// Created by andres on 6/10/25.
//

#ifndef GUI_CODECOACH_RUNRESULTSWIDGETS_H
#define GUI_CODECOACH_RUNRESULTSWIDGETS_H

#pragma once
#include <QWidget>
#include "../viewmodels/dto/RunResults.h"

class QPlainTextEdit;
class QLabel;

class RunResultsWidget : public QWidget {
    Q_OBJECT
public:
    explicit RunResultsWidget(QWidget* parent=nullptr);
    void showRunning();

public slots:
    void render(const cc::dto::RunResults& r);
    void appendStdOut(const QString& line);
    void appendStdErr(const QString& line);

private:
    QLabel* statusLbl_ = nullptr;
    QPlainTextEdit* out_ = nullptr;
    QPlainTextEdit* err_ = nullptr;
};


#endif //GUI_CODECOACH_RUNRESULTSWIDGETS_H //para mostrar los resultados