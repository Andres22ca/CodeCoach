

#ifndef GUI_CODECOACH_PROBLEMDETAILWIDGET_H
#define GUI_CODECOACH_PROBLEMDETAILWIDGET_H

#include <QWidget>
#include <QTextBrowser>
#include "../viewmodels/dto/ProblemDetail.h"

class ProblemDetailWidget : public QTextBrowser {
    Q_OBJECT;

public:
    explicit ProblemDetailWidget(QWidget* parent = nullptr);

public:
    void render(const cc::dto::ProblemDetail& d);
};

#endif // GUI_CODECOACH_PROBLEMDETAILWIDGET_H

