cmake_minimum_required(VERSION 3.20)
project(gui_codecoach LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Necesario para Q_OBJECT, .ui, .qrc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt6 6.4.2 REQUIRED COMPONENTS Widgets Network)



add_executable(gui_codecoach
        main.cpp

        # Main window
        mainwindow/MainWindow.cpp
        mainwindow/MainWindow.h

        # Components
        components/ProblemListWidget.cpp
        components/ProblemListWidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/components/ProblemDetailWidget.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/components/ProblemDetailWidget.h
        components/CodeEditorWidget.cpp
        components/CodeEditorWidget.h
        components/RunResultsWidget.cpp
        components/RunResultsWidget.h
        components/CoachFeedbackWidget.cpp
        components/CoachFeedbackWidget.h

        # ViewModels
        viewmodels/ProblemViewModel.cpp
        viewmodels/ProblemViewModel.h
        viewmodels/EditorViewModel.cpp
        viewmodels/EditorViewModel.h
        viewmodels/RunViewModel.cpp
        viewmodels/RunViewModel.h
        viewmodels/CoachViewModel.cpp
        viewmodels/CoachViewModel.h
        viewmodels/dto/ProblemSummary.h
        viewmodels/dto/ProblemDetail.h
        viewmodels/dto/RunResults.h
        viewmodels/dto/CoachFeedback.h
)

set_target_properties(gui_codecoach PROPERTIES
        AUTOMOC ON
        AUTOUIC ON
        AUTORCC ON
)

target_link_libraries(gui_codecoach
        PRIVATE
        Qt6::Widgets
        Qt6::Network
)

