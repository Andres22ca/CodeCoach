#pragma once
#ifndef GUI_CODECOACH_PROBLEMLISTWIDGET_H
#define GUI_CODECOACH_PROBLEMLISTWIDGET_H

#include <QListWidget>
#include <QVector>
#include <QString>

class QListWidgetItem;   // forward declaration

namespace cc::dto {
    struct ProblemSummary;
    struct ProblemDetail;
}

class ProblemListWidget : public QListWidget {
    Q_OBJECT
public:
    explicit ProblemListWidget(QWidget* parent = nullptr);

public slots:
    void setProblems(const QVector<cc::dto::ProblemSummary>& list);

    signals:
        void problemChosen(const cc::dto::ProblemDetail& d);

private slots:
    void onItemClick(QListWidgetItem* it);

private:
    QVector<cc::dto::ProblemDetail> problems_;
};

#endif // GUI_CODECOACH_PROBLEMLISTWIDGET_H
