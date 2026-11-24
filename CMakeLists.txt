cmake_minimum_required(VERSION 3.16)

project(Tree VERSION 0.1 LANGUAGES CXX)

# --- Общие настройки C++ ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt: AUTOMOC нужен для Q_OBJECT в .h (NodeItem, TreeScene, …)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

# --- Поиск Qt: сначала Qt6, потом Qt5 ---
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# =====================================================================
#  Библиотека с ядром и UI (всё, кроме main.cpp и тестов)
# =====================================================================
set(TREE_CORE_SOURCES
    # Деревья
    avltree.cpp        avltree.h
    btree.cpp          btree.h
    rbtree.cpp         rbtree.h
    splaytree.cpp      splaytree.h
    treap.cpp          treap.h

    # Контроллер / хранилище / раскладка / плеер шагов
    treecontroller.cpp treecontroller.h
    treestore.cpp      treestore.h
    treelayout.cpp     treelayout.h
    stepplayer.cpp     stepplayer.h

    # Графическая сцена и элементы
    treescene.cpp      treescene.h
    nodeitem.cpp       nodeitem.h
    edgeitem.cpp       edgeitem.h

    # Окно
    mainwindow.cpp     mainwindow.h

    # Общие заголовки (нужны AUTOMOC'у, даже без .cpp)
    appsettings.h
    easing.h
    history.h
    itree.h
    nodebase.h
    operationsteps.h
    snapshot.h
    theme.h
    treetypes.h
)

add_library(tree_core STATIC ${TREE_CORE_SOURCES})
set_target_properties(tree_core PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(tree_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(tree_core PUBLIC Qt${QT_VERSION_MAJOR}::Widgets)

# =====================================================================
#  Основное приложение
# =====================================================================
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE tree_core)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# =====================================================================
#  Тесты (опционально, выключаются -DBUILD_TESTS=OFF)
# =====================================================================
option(BUILD_TESTS "Собирать тесты (tests_avl, tests_rb)" ON)

