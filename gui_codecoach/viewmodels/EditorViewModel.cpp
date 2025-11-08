
#include "EditorViewModel.h"

using namespace cc::vm;

EditorViewModel::EditorViewModel(QObject* parent): QObject(parent) {}

void EditorViewModel::loadStarterFor(const QString& id){
    emit codeReady(QString(
       "// %1\n#include <bits/stdc++.h>\nusing namespace std;\nint main(){return 0;}\n").arg(id));
}
