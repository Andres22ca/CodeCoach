#ifndef CODECOACH_RUNVIEWMODEL_H
#define CODECOACH_RUNVIEWMODEL_H

#include <QObject>
#include <QString>
#include "dto/RunResults.h"

namespace cc::dto {
    struct RunResults;
}

namespace cc::vm {

    class RunViewModel : public QObject {
        Q_OBJECT
    public:
        explicit RunViewModel(QObject* parent = nullptr);

        // MainWindow lo llama así: run(pid, code)
        void run(const QString& problemId, const QString& code);

        // ★ ESTA ES LA QUE FALTABA
        void cancel();

        void clear();

        cc::dto::RunResults results() const;
        bool isRunning() const;
        int  progress() const;

        signals:
            void runningChanged(bool running);
        void progressChanged(int progress);
        void resultsReady(const cc::dto::RunResults& results);
        void errorOccurred(const QString& message);

        void stdOut(const QString& text);
        void stdErr(const QString& text);

    private:
        void setRunning(bool running);
        void setProgress(int p);

        bool running_ = false;
        int  progress_ = 0;
        cc::dto::RunResults lastResults_;
    };

} // namespace cc::vm

#endif // CODECOACH_RUNVIEWMODEL_H
