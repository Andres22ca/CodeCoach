

#include "ProblemViewModel.h"

using namespace cc::vm;
using cc::dto::ProblemSummary;
using cc::dto::ProblemDetail;

ProblemViewModel::ProblemViewModel(QObject* parent) : QObject(parent) {}

void ProblemViewModel::loadMock() {
    list_.clear();
    list_.append({ "two-sum", "Two Sum", "Easy", {"array","hash"} });
    list_.append({ "merge-intervals", "Merge Intervals", "Medium", {"intervals","sort"} });
    list_.append({ "word-ladder", "Word Ladder", "Hard", {"bfs","graph"} });
    emit problemsReady(list_);
    setCurrentById(list_.front().id);
}

void ProblemViewModel::setCurrentById(const QString& id) {
    if (id.isEmpty()) return;
    ProblemDetail d;
    d.id = id;
    for (const auto& p : list_) if (p.id==id){ d.title=p.title; d.difficulty=p.difficulty; d.tags=p.tags; break; }
    d.examples = {"input: ...", "output: ..."};
    d.statementHtml = QString("<p>Enunciado para <i>%1</i>…</p>").arg(d.title);
    emit detailReady(d);
}
