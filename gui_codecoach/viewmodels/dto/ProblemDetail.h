

#ifndef CODECOACH_PROBLEMDETAIL_H
#define CODECOACH_PROBLEMDETAIL_H

#include <QString>
#include <QStringList>

namespace cc::dto {

    struct ProblemDetail {
        QString id;                // ID del problema
        QString title;             // Título
        QString statementHtml;     // Enunciado (HTML o Markdown renderizable)
        QString difficulty;        // Nivel
        QStringList examples;      // Casos de ejemplo
        QStringList tags;          // Categorías
    };

} // namespace cc::dto

#endif // CODECOACH_PROBLEMDETAIL_H
