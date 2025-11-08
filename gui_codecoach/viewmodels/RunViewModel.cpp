
#include "RunViewModel.h"
#include <QTimer>
using namespace cc::vm;
using cc::dto::RunResults;

RunViewModel::RunViewModel(QObject* parent): QObject(parent) {}

void RunViewModel::run(const QString& /*problemId*/, const QString& /*code*/) {
    emit stdOut("[runner] Compilando…");
    QTimer::singleShot(400, this, [this]{
        RunResults r;
        r.status = RunResults::Status::Failed;
        r.testsPassed = 1; r.testsTotal = 2;
        r.timeMs = 95.3; r.memoryBytes = 8'400'000;
        r.stdOut = "ok case #1\n";
        r.stdErr = "assert fail at case #2\n";
        r.failedCases = {2};
        emit resultsReady(r);
    });
}
