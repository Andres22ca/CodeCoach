#ifndef CC_DTO_PROBLEMSUMMARY_H
#define CC_DTO_PROBLEMSUMMARY_H

#include <QString>
#include <QStringList>
#include <QMetaType>
#include <QList>

namespace cc::dto {

    struct ProblemSummary {
        QString id;
        QString title;
        QString difficulty;
        QStringList tags;
    };

} // namespace cc::dto

// Metatipos para Qt
Q_DECLARE_METATYPE(cc::dto::ProblemSummary)
Q_DECLARE_METATYPE(QList<cc::dto::ProblemSummary>)

#endif // CC_DTO_PROBLEMSUMMARY_H


