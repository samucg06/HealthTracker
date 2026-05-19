#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

/**
 * @file LoginDialog.h
 * @brief Diálogo de autenticación y registro de usuarios.
 */

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLabel>
#include "database/DatabaseManager.h"

/**
 * @class LoginDialog
 * @brief Ventana modal con pestañas para iniciar sesión o registrarse.
 *
 * Al aceptar, `getLoggedUser()` devuelve el UserRecord del usuario autenticado.
 */
class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget* parent = nullptr);

    /// Retorna el usuario autenticado (válido sólo si exec() == Accepted).
    UserRecord getLoggedUser() const { return m_user; }

private slots:
    void onLogin();
    void onRegister();

private:
    void setupUi();

    // ── Campos de Login ───────────────────────
    QLineEdit* m_loginUser;
    QLineEdit* m_loginPass;

    // ── Campos de Registro ────────────────────
    QLineEdit*       m_regFullName;
    QLineEdit*       m_regUser;
    QLineEdit*       m_regPass;
    QSpinBox*        m_regBirthYear;
    QDoubleSpinBox*  m_regHeight;
    QComboBox*       m_regGender;

    QLabel*    m_statusLabel;
    UserRecord m_user;
};

#endif // LOGINDIALOG_H
