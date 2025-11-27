#ifndef GUI_CODECOACH_PROBLEMLISTWIDGET_H
#define GUI_CODECOACH_PROBLEMLISTWIDGET_H

#include <QListWidget>
#include "../viewmodels/dto/ProblemSummary.h"

class ProblemListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit ProblemListWidget(QWidget* parent = nullptr);

public slots:
    void setProblems(const QList<cc::dto::ProblemSummary>& problems);

    signals:
        void problemChosen(const QString& problemId);

private slots:
    void handleItemClicked(QListWidgetItem* item);
};

#endif // GUI_CODECOACH_PROBLEMLISTWIDGET_H

