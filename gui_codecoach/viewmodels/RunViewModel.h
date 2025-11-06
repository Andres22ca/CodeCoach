//
// Created by andres on 6/10/25.
//


#ifndef CODECOACH_RUNVIEWMODEL_H
#define CODECOACH_RUNVIEWMODEL_H

#include <QObject>
#include <QString>
#include "dto/RunResults.h"

namespace cc::vm {

    class RunViewModel : public QObject {
        Q_OBJECT
    public:
        explicit RunViewModel(QObject* parent = nullptr);

        // --- API pública ---
        void run(const QString& code, const QString& problemId);  // Inicia una ejecución
        void cancel();                                            // Cancela la ejecución actual
        void clear();                                             // Limpia resultados previos

        // --- Accesores ---
        cc::dto::RunResults results() const;                      // Devuelve el último resultado
        bool isRunning() const;                                   // Indica si hay ejecución activa
        int progress() const;                                     // Progreso (0-100%)

        signals:
            // --- Señales hacia la UI ---
            void runningChanged(bool running);                        // Comienza o termina ejecución
        void progressChanged(int progress);                       // Progreso incremental
        void resultsReady(const cc::dto::RunResults& results);    // Resultados listos
        void errorOccurred(const QString& message);               // Error genérico

    private:
        // --- Estado interno ---
        bool running_ = false;
        int progress_ = 0;
        cc::dto::RunResults lastResults_;
    };

} // namespace cc::vm

#endif // CODECOACH_RUNVIEWMODEL_H
