#include "ProblemListWidget.h"
#include <QListWidgetItem>

#include "../viewmodels/dto/ProblemSummary.h"
#include "../viewmodels/dto/ProblemDetail.h"

// ajusta la ruta a donde estén realmente

using cc::dto::ProblemSummary;
using cc::dto::ProblemDetail;

ProblemListWidget::ProblemListWidget(QWidget* parent)
    : QListWidget(parent)
{
    connect(this, &QListWidget::itemClicked,
            this, &ProblemListWidget::onItemClick);
}

void ProblemListWidget::setProblems(const QVector<ProblemSummary>& list) {
    clear();
    problems_.clear();
    problems_.reserve(list.size());

    for (const auto& p : list) {
        auto* item = new QListWidgetItem(p.title, this); // asumo title
        item->setData(Qt::UserRole, p.id);               // asumo id
        addItem(item);
    }
}

void ProblemListWidget::onItemClick(QListWidgetItem* it) {
    const QString id = it->data(Qt::UserRole).toString();

    ProblemDetail d;
    d.id    = id;
    d.title = it->text();

    emit problemChosen(d);
}
