/**
 * @file main.cpp
 * @brief Punto de entrada de HealthTracker.
 *
 * Inicializa la base de datos SQLite3, muestra el diálogo de
 * autenticación y lanza la ventana principal.
 *
 * @mainpage HealthTracker — Rastreador Personal de Salud
 *
 * @section intro Descripción
 * Aplicación de escritorio desarrollada en C++17 con Qt5 y SQLite3.
 * Permite registrar mediciones biométricas (peso, presión arterial,
 * glucosa), calcular estadísticas, visualizar tendencias y estimar
 * las kilocalorías diarias con la ecuación de Mifflin-St Jeor.
 *
 * @section arch Arquitectura
 * - **DatabaseManager** (Singleton): acceso a SQLite3.
 * - **StatsCalculator**: métodos estáticos de análisis.
 * - **LoginDialog**: autenticación / registro.
 * - **MainWindow**: ventana principal con tabla y gráfica.
 * - **AddMeasurementDialog**: formulario de nueva medición.
 * - **CalorieDialog**: calculadora de kcal diarias.
 */

#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QMessageBox>

#include "database/DatabaseManager.h"
#include "ui/LoginDialog.h"
#include "ui/MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Metadatos de la aplicación
    app.setApplicationName("HealthTracker");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("ECI");

    // ── Ruta de la base de datos ──────────────────
    // ~/.local/share/ECI/HealthTracker/health.db  (Linux)
    QString dataDir = QStandardPaths::writableLocation(
        QStandardPaths::AppDataLocation);
    if (!QDir().mkpath(dataDir)) {
        QMessageBox::critical(nullptr, "Error",
            "No se pudo crear el directorio de datos.");
        return 1;
    }

    const QString dbPath = dataDir + "/health.db";

    if (!DatabaseManager::instance().open(dbPath)) {
        QMessageBox::critical(nullptr, "Error de base de datos",
            "No se pudo abrir la base de datos:\n" + dbPath);
        return 1;
    }

    // ── Autenticación ─────────────────────────────
    LoginDialog loginDlg;
    if (loginDlg.exec() != QDialog::Accepted) {
        return 0;   // El usuario cerró el diálogo
    }

    // ── Ventana principal ─────────────────────────
    MainWindow window(loginDlg.getLoggedUser());
    window.show();

    return app.exec();
}
