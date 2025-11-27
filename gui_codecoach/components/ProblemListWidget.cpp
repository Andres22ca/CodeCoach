#include "ProblemListWidget.h"

ProblemListWidget::ProblemListWidget(QWidget* parent)
    : QListWidget(parent)
{
    connect(this, &QListWidget::itemClicked,
            this, &ProblemListWidget::handleItemClicked);
}

void ProblemListWidget::setProblems(const QList<cc::dto::ProblemSummary>& problems)
{
    clear();
    for (const auto& p : problems) {
        auto* it = new QListWidgetItem(p.title, this);
        it->setData(Qt::UserRole, p.id);
    }
}

void ProblemListWidget::handleItemClicked(QListWidgetItem* item)
{
    const QString id = item->data(Qt::UserRole).toString();
    emit problemChosen(id);
}
