cmake_minimum_required(VERSION 4.0)
project(gui_codecoach)

set(CMAKE_CXX_STANDARD 20)

add_executable(gui_codecoach main.cpp
        components/ProblemListWidget.cpp
        components/ProblemListWidget.h
        components/ProblemDetailWidget.cpp
        components/ProblemDetailWidget.h
        components/CodeEditorWidget.cpp
        components/CodeEditorWidget.h
        components/RunResultsWidgets.cpp
        components/RunResultsWidgets.h
        components/CoachFeedbackWidget.cpp
        components/CoachFeedbackWidget.h
        viewmodels/ProblemViewModel.cpp
        viewmodels/ProblemViewModel.h
        viewmodels/EditorViewmodel.cpp
        viewmodels/EditorViewmodel.h
        viewmodels/RunViewModel.cpp
        viewmodels/RunViewModel.h
        viewmodels/CoachViewModel.cpp
        viewmodels/CoachViewModel.h)
