cmake_minimum_required(VERSION 3.20)
project(gui_codecoach LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
        components/ProblemDetailWidget.cpp
        components/ProblemDetailWidget.h
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

# Para que la GUI pueda incluir headers de lib_codecoach
target_include_directories(gui_codecoach
        PRIVATE
        ${CMAKE_SOURCE_DIR}/lib_codecoach
)

target_link_libraries(gui_codecoach
        PRIVATE
        Qt6::Widgets
        Qt6::Network
        lib_codecoach      # <--- AQUÍ enlazamos la librería
)
