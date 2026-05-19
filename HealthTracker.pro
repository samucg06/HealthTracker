# ============================================================
#  HealthTracker.pro
#  Rastreador Personal de Salud
#  Universidad ECI - Algoritmos en Sistemas Electrónicos
# ============================================================

QT += core gui widgets charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET   = HealthTracker
TEMPLATE = app

CONFIG   += c++17
CONFIG   -= debug_and_release

# Ruta de includes
INCLUDEPATH += src

SOURCES += \
    src/main.cpp \
    src/database/DatabaseManager.cpp \
    src/ui/LoginDialog.cpp \
    src/ui/MainWindow.cpp \
    src/ui/AddMeasurementDialog.cpp \
    src/ui/CalorieDialog.cpp \
    src/utils/StatsCalculator.cpp

HEADERS += \
    src/database/DatabaseManager.h \
    src/ui/LoginDialog.h \
    src/ui/MainWindow.h \
    src/ui/AddMeasurementDialog.h \
    src/ui/CalorieDialog.h \
    src/utils/StatsCalculator.h

# Enlazar SQLite3
LIBS += -lsqlite3

# Optimización de tamaño del ejecutable (requerido por rúbrica)
CONFIG(release, debug|release): QMAKE_CXXFLAGS += -Os
