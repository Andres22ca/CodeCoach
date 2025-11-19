#include "ProblemDetailWidget.h"
#include <QString>

ProblemDetailWidget::ProblemDetailWidget(QWidget* parent)
    : QTextBrowser(parent)
{
}

void ProblemDetailWidget::render(const cc::dto::ProblemDetail& d) {
    QString ex;
    for (const auto& e : d.examples) {
        ex += "<pre>" + e.toHtmlEscaped() + "</pre>";
    }

    QString tg = d.tags.join(", ");

    setHtml(QString(
        "<h2>%1</h2><p><b>Dificultad:</b> %2"
        "<br><b>Tags:</b> %3</p>%4"
        "<h3>Enunciado</h3>%5")
        .arg(d.title, d.difficulty, tg, ex, d.statementHtml));
};

