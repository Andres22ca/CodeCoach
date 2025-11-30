#include "ProblemDetailWidget.h"
#include <QString>

ProblemDetailWidget::ProblemDetailWidget(QWidget* parent)
    : QTextBrowser(parent)
{
}

void ProblemDetailWidget::render(const cc::dto::ProblemDetail& d) {
    // bloque de ejemplos
    QString ex;
    for (const auto& e : d.examples) {
        ex += "<pre>" + e.toHtmlEscaped() + "</pre>";
    }

    // Tags
    QString tg = d.tags.join(", ");


    QString html = QString(
        "<h2>%1</h2>"
        "<p><b>Dificultad:</b> %2"
        "<br><b>Tags:</b> %3</p>"
        "%4"
        "<h3>Enunciado</h3>%5"
    ).arg(
        d.title.toHtmlEscaped(),       // %1
        d.difficulty.toHtmlEscaped(),  // %2
        tg.toHtmlEscaped(),            // %3
        ex,                            // %4 (ya tiene <pre>…</pre>)
        d.statementHtml                // %5 (HTML ya preparado)
    );

    setHtml(html);
}
