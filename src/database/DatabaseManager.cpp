/**
 * @file DatabaseManager.cpp
 * @brief Implementación del gestor de base de datos SQLite3.
 */

#include "DatabaseManager.h"
#include <QCryptographicHash>
#include <QDebug>

// ─────────────────────────────────────────────
//  Singleton
// ─────────────────────────────────────────────

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager inst;
    return inst;
}

// ─────────────────────────────────────────────
//  Conexión
// ─────────────────────────────────────────────

bool DatabaseManager::open(const QString& path) {
    int rc = sqlite3_open(path.toUtf8().constData(), &m_db);
    if (rc != SQLITE_OK) {
        qWarning() << "[DB] No se pudo abrir la base de datos:" << sqlite3_errmsg(m_db);
        sqlite3_close(m_db);
        m_db = nullptr;
        return false;
    }
    // Habilitar foreign keys
    sqlite3_exec(m_db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
    return createTables();
}

void DatabaseManager::close() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

// ─────────────────────────────────────────────
//  Creación de tablas
// ─────────────────────────────────────────────

bool DatabaseManager::createTables() {
    const char* sql = R"SQL(
        CREATE TABLE IF NOT EXISTS users (
            id        INTEGER PRIMARY KEY AUTOINCREMENT,
            username  TEXT    UNIQUE NOT NULL,
            password  TEXT    NOT NULL,
            full_name TEXT    DEFAULT '',
            birth_year INTEGER DEFAULT 2000,
            height_cm  REAL    DEFAULT 170.0,
            gender     TEXT    DEFAULT 'Otro'
        );

        CREATE TABLE IF NOT EXISTS measurements (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id     INTEGER NOT NULL,
            type        TEXT    NOT NULL,
            value1      REAL    NOT NULL DEFAULT 0,
            value2      REAL             DEFAULT 0,
            recorded_at TEXT    NOT NULL,
            notes       TEXT             DEFAULT '',
            FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
        );
    )SQL";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        qWarning() << "[DB] Error al crear tablas:" << errMsg;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// ─────────────────────────────────────────────
//  Utilidad: hash de contraseña (SHA-256)
// ─────────────────────────────────────────────

static QString hashPassword(const QString& password) {
    return QString(
        QCryptographicHash::hash(password.toUtf8(),
                                 QCryptographicHash::Sha256).toHex()
    );
}

// ─────────────────────────────────────────────
//  Operaciones de Usuario
// ─────────────────────────────────────────────

bool DatabaseManager::registerUser(const QString& username,
                                    const QString& password,
                                    const QString& fullName,
                                    int birthYear,
                                    double heightCm,
                                    const QString& gender) {
    const char* sql =
        "INSERT INTO users (username, password, full_name, birth_year, height_cm, gender) "
        "VALUES (?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        qWarning() << "[DB] registerUser prepare error:" << sqlite3_errmsg(m_db);
        return false;
    }

    sqlite3_bind_text  (stmt, 1, username.toUtf8().constData(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 2, hashPassword(password).toUtf8().constData(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 3, fullName.toUtf8().constData(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_int   (stmt, 4, birthYear);
    sqlite3_bind_double(stmt, 5, heightCm);
    sqlite3_bind_text  (stmt, 6, gender.toUtf8().constData(),    -1, SQLITE_TRANSIENT);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

bool DatabaseManager::loginUser(const QString& username,
                                 const QString& password,
                                 UserRecord& outUser) {
    const char* sql =
        "SELECT id, username, full_name, birth_year, height_cm, gender "
        "FROM users WHERE username = ? AND password = ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        qWarning() << "[DB] loginUser prepare error:" << sqlite3_errmsg(m_db);
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.toUtf8().constData(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, hashPassword(password).toUtf8().constData(), -1, SQLITE_TRANSIENT);

    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        outUser.id        = sqlite3_column_int   (stmt, 0);
        outUser.username  = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        outUser.fullName  = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
        outUser.birthYear = sqlite3_column_int   (stmt, 3);
        outUser.heightCm  = sqlite3_column_double(stmt, 4);
        outUser.gender    = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
        found = true;
    }
    sqlite3_finalize(stmt);
    return found;
}

// ─────────────────────────────────────────────
//  Operaciones de Medición
// ─────────────────────────────────────────────

bool DatabaseManager::addMeasurement(int userId,
                                      const QString& type,
                                      double value1, double value2,
                                      const QString& recordedAt,
                                      const QString& notes) {
    const char* sql =
        "INSERT INTO measurements (user_id, type, value1, value2, recorded_at, notes) "
        "VALUES (?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int   (stmt, 1, userId);
    sqlite3_bind_text  (stmt, 2, type.toUtf8().constData(),       -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, value1);
    sqlite3_bind_double(stmt, 4, value2);
    sqlite3_bind_text  (stmt, 5, recordedAt.toUtf8().constData(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 6, notes.toUtf8().constData(),      -1, SQLITE_TRANSIENT);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

QVector<MeasurementRecord> DatabaseManager::getMeasurements(int userId,
                                                             const QString& type) {
    QVector<MeasurementRecord> results;

    QString sqlStr =
        "SELECT id, user_id, type, value1, value2, recorded_at, notes "
        "FROM measurements WHERE user_id = ?";
    if (!type.isEmpty()) sqlStr += " AND type = ?";
    sqlStr += " ORDER BY recorded_at DESC;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sqlStr.toUtf8().constData(), -1, &stmt, nullptr) != SQLITE_OK)
        return results;

    sqlite3_bind_int(stmt, 1, userId);
    if (!type.isEmpty())
        sqlite3_bind_text(stmt, 2, type.toUtf8().constData(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        MeasurementRecord r;
        r.id         = sqlite3_column_int   (stmt, 0);
        r.userId     = sqlite3_column_int   (stmt, 1);
        r.type       = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
        r.value1     = sqlite3_column_double(stmt, 3);
        r.value2     = sqlite3_column_double(stmt, 4);
        r.recordedAt = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
        r.notes      = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
        results.append(r);
    }
    sqlite3_finalize(stmt);
    return results;
}

bool DatabaseManager::deleteMeasurement(int id) {
    const char* sql = "DELETE FROM measurements WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, id);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}
