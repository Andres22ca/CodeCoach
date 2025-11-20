#pragma once
#ifndef GUI_CODECOACH_PROBLEMLISTWIDGET_H
#define GUI_CODECOACH_PROBLEMLISTWIDGET_H

#include <QListWidget>
#include <QString>

class QListWidgetItem;

namespace cc::dto {
    struct ProblemSummary;
}

class ProblemListWidget : public QListWidget {
    Q_OBJECT
public:
    explicit ProblemListWidget(QWidget* parent = nullptr);

public slots:
    void setProblems(const QVector<cc::dto::ProblemSummary>& list);

    signals:
        void problemChosen(const QString& id);   // <-- SOLO id

private slots:
    void onItemClick(QListWidgetItem* it);
};

#endif

