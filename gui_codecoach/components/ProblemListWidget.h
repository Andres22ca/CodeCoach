

#ifndef GUI_CODECOACH_PROBLEMLISTWIDGET_H
#define GUI_CODECOACH_PROBLEMLISTWIDGET_H

#pragma once
#include <QListWidget>
#include "../viewmodels/dto/ProblemSummary.h"

class ProblemListWidget : public QListWidget {
    Q_OBJECT
public:
    explicit ProblemListWidget(QWidget* parent=nullptr);

public slots:
    void setProblems(const QVector<cc::dto::ProblemSummary>& list);

    signals:
        void problemChosen(QString id);

private slots:
    void onItemClick(QListWidgetItem* it);
};


#endif //GUI_CODECOACH_PROBLEMLISTWIDGET_H