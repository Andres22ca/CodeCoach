//
// Created by andres on 6/10/25.
//

#ifndef GUI_CODECOACH_PROBLEMDETAILWIDGET_H
#define GUI_CODECOACH_PROBLEMDETAILWIDGET_H

#pragma once
#include <QTextBrowser>
#include "../viewmodels/dto/ProblemDetail.h"

class ProblemDetailWidget : public QTextBrowser {
    Q_OBJECT
public:
    explicit ProblemDetailWidget(QWidget* parent=nullptr);

public slots:
    void render(const cc::dto::ProblemDetail& d);
};



#endif //GUI_CODECOACH_PROBLEMDETAILWIDGET_H
// lista de prblemas