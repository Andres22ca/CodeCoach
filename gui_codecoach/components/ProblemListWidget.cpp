
#include "ProblemListWidget.h"
using cc::dto::ProblemSummary;

ProblemListWidget::ProblemListWidget(QWidget* parent) : QListWidget(parent) {
    connect(this, &QListWidget::itemClicked, this, &ProblemListWidget::onItemClick);
}

void ProblemListWidget::setProblems(const QVector<ProblemSummary>& list) {
    clear();
    for (const auto& p : list) {
        auto* it = new QListWidgetItem(QString("[%1] %2").arg(p.difficulty, p.title), this);
        it->setData(Qt::UserRole, p.id);
    }
}

void ProblemListWidget::onItemClick(QListWidgetItem* it) {
    emit problemChosen(it->data(Qt::UserRole).toString());
}
