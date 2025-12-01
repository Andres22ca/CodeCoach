#ifndef CODECOACH_PROBLEMVIEWMODEL_H
#define CODECOACH_PROBLEMVIEWMODEL_H

#pragma once

#include <QObject>
#include <QVector>

#include "dto/ProblemSummary.h"
#include "dto/ProblemDetail.h"

#include "sdk/problems_client.h"

namespace cc::vm {

    class ProblemViewModel : public QObject {
        Q_OBJECT
    public:
        explicit ProblemViewModel(QObject* parent = nullptr);

    public slots:
        // Por ahora sigue llamándose loadMock,
        // pero internamente lo iremos cambiando para que
        // use el servicio REST en lugar de datos fake.
        void loadMock();
        void setCurrentById(const QString& id);

        signals:
            void problemsReady(QVector<cc::dto::ProblemSummary> list);
        void detailReady(cc::dto::ProblemDetail detail);

    private:
        QVector<cc::dto::ProblemSummary> list_;

        // 👇 Cliente real que habla con el microservicio de problemas
        //    http://localhost:9001 es solo un ejemplo; luego lo podemos leer de config.
        cc::sdk::ProblemsClient problemsClient_{"http://localhost:9001"};
    };

} // namespace cc::vm

#endif // CODECOACH_PROBLEMVIEWMODEL_H

