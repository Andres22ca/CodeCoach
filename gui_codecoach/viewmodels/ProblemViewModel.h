

#ifndef CODECOACH_PROBLEMVIEWMODEL_H
#define CODECOACH_PROBLEMVIEWMODEL_H

#pragma once
#include <QObject>
#include <QVector>
#include "dto/ProblemSummary.h"
#include "dto/ProblemDetail.h"

namespace cc::vm {
    class ProblemViewModel : public QObject {
        Q_OBJECT
    public:
        explicit ProblemViewModel(QObject* parent=nullptr);

    public slots:
        void loadMock();
        void setCurrentById(const QString& id);

        signals:
            void problemsReady(QVector<cc::dto::ProblemSummary> list);
        void detailReady(cc::dto::ProblemDetail detail);

    private:
        QVector<cc::dto::ProblemSummary> list_;
    };
}


#endif // CODECOACH_PROBLEMVIEWMODEL_H
