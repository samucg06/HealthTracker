#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

/**
 * @file DatabaseManager.h
 * @brief Gestión de la base de datos SQLite3 para HealthTracker.
 * @author Ingeniería Electrónica - ECI
 */

#include <QString>
#include <QVector>
#include <sqlite3.h>

// ─────────────────────────────────────────────
//  Estructuras de datos (modelos simples)
// ─────────────────────────────────────────────

/**
 * @struct UserRecord
 * @brief Representa un usuario registrado en el sistema.
 */
struct UserRecord {
    int     id        = 0;
    QString username;
    QString fullName;
    int     birthYear = 2000;
    double  heightCm  = 170.0;
    QString gender;
};

/**
 * @struct MeasurementRecord
 * @brief Representa una medición biométrica almacenada.
 */
struct MeasurementRecord {
    int     id         = 0;
    int     userId     = 0;
    QString type;        ///< "weight" | "blood_pressure" | "glucose"
    double  value1     = 0.0;
    double  value2     = 0.0;  ///< Usado en presión arterial (diastólica)
    QString recordedAt;
    QString notes;
};

// ─────────────────────────────────────────────
//  Clase DatabaseManager (Singleton)
// ─────────────────────────────────────────────

/**
 * @class DatabaseManager
 * @brief Singleton que encapsula todas las operaciones sobre SQLite3.
 *
 * Patrón de uso:
 * @code
 *   DatabaseManager::instance().open("health.db");
 *   DatabaseManager::instance().registerUser(...);
 * @endcode
 */
class DatabaseManager {
public:
    /// Retorna la instancia única (Singleton).
    static DatabaseManager& instance();

    bool open(const QString& path);
    void close();
    bool isOpen() const { return m_db != nullptr; }

    // ── Usuarios ──────────────────────────────
    bool registerUser(const QString& username, const QString& password,
                      const QString& fullName,  int birthYear,
                      double heightCm,          const QString& gender);

    bool loginUser(const QString& username, const QString& password,
                   UserRecord& outUser);

    // ── Mediciones ────────────────────────────
    bool addMeasurement(int userId,         const QString& type,
                        double value1,      double value2,
                        const QString& recordedAt, const QString& notes);

    QVector<MeasurementRecord> getMeasurements(int userId,
                                               const QString& type = "");

    bool deleteMeasurement(int id);

private:
    DatabaseManager()  = default;
    ~DatabaseManager() { close(); }

    // No copiable
    DatabaseManager(const DatabaseManager&)            = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool createTables();

    sqlite3* m_db = nullptr;
};

#endif // DATABASEMANAGER_H
