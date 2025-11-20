#include "ProblemListWidget.h"
#include <QListWidgetItem>

#include "../viewmodels/dto/ProblemSummary.h"
using cc::dto::ProblemSummary;

ProblemListWidget::ProblemListWidget(QWidget* parent)
    : QListWidget(parent)
{
    connect(this, &QListWidget::itemClicked,
            this, &ProblemListWidget::onItemClick);
}

void ProblemListWidget::setProblems(const QVector<ProblemSummary>& list) {
    clear();

    for (const auto& p : list) {
        auto* item = new QListWidgetItem(p.title, this);
        item->setData(Qt::UserRole, p.id);
        addItem(item);
    }
}

void ProblemListWidget::onItemClick(QListWidgetItem* it) {
    const QString id = it->data(Qt::UserRole).toString();
    emit problemChosen(id);     // <-- SOLO id
}
