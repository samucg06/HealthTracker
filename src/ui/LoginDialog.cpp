/**
 * @file LoginDialog.cpp
 * @brief Implementación del diálogo de autenticación.
 */

#include "LoginDialog.h"
#include "database/DatabaseManager.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QMessageBox>

// ─────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("HealthTracker — Acceso");
    setFixedWidth(400);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setupUi();
}

// ─────────────────────────────────────────────
//  Construcción de la UI
// ─────────────────────────────────────────────

void LoginDialog::setupUi() {
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setSpacing(10);
    rootLayout->setContentsMargins(20, 20, 20, 15);

    // ── Título ────────────────────────────────
    auto* titleLabel = new QLabel(
        "<h2 style='color:#2a7; margin:0'>🏥 HealthTracker</h2>"
        "<p style='color:gray; margin:0; font-size:10pt'>"
        "Rastreador Personal de Salud</p>");
    titleLabel->setAlignment(Qt::AlignCenter);
    rootLayout->addWidget(titleLabel);

    // ── Pestañas ──────────────────────────────
    auto* tabs = new QTabWidget(this);

    // ─── Pestaña: Iniciar sesión ──────────────
    {
        auto* page = new QWidget();
        auto* form = new QFormLayout(page);
        form->setSpacing(10);
        form->setContentsMargins(10, 15, 10, 10);

        m_loginUser = new QLineEdit();
        m_loginUser->setPlaceholderText("Ej: jgomez");
        m_loginPass = new QLineEdit();
        m_loginPass->setEchoMode(QLineEdit::Password);
        m_loginPass->setPlaceholderText("••••••");

        form->addRow("Usuario:", m_loginUser);
        form->addRow("Contraseña:", m_loginPass);

        auto* btn = new QPushButton("Ingresar");
        btn->setDefault(true);
        btn->setMinimumHeight(32);
        connect(btn, &QPushButton::clicked, this, &LoginDialog::onLogin);
        form->addRow(btn);

        tabs->addTab(page, "Iniciar sesión");
    }

    // ─── Pestaña: Registrarse ─────────────────
    {
        auto* page = new QWidget();
        auto* form = new QFormLayout(page);
        form->setSpacing(8);
        form->setContentsMargins(10, 15, 10, 10);

        m_regFullName = new QLineEdit();
        m_regFullName->setPlaceholderText("Nombre y apellido");
        m_regUser = new QLineEdit();
        m_regUser->setPlaceholderText("Sin espacios ni tildes");
        m_regPass = new QLineEdit();
        m_regPass->setEchoMode(QLineEdit::Password);
        m_regPass->setPlaceholderText("Mínimo 4 caracteres");

        m_regBirthYear = new QSpinBox();
        m_regBirthYear->setRange(1920, 2010);
        m_regBirthYear->setValue(2003);

        m_regHeight = new QDoubleSpinBox();
        m_regHeight->setRange(100.0, 250.0);
        m_regHeight->setValue(170.0);
        m_regHeight->setSuffix(" cm");
        m_regHeight->setDecimals(1);

        m_regGender = new QComboBox();
        m_regGender->addItems({"Masculino", "Femenino", "Otro"});

        form->addRow("Nombre completo:", m_regFullName);
        form->addRow("Usuario:",         m_regUser);
        form->addRow("Contraseña:",      m_regPass);
        form->addRow("Año de nacimiento:", m_regBirthYear);
        form->addRow("Estatura:",        m_regHeight);
        form->addRow("Género:",          m_regGender);

        auto* btn = new QPushButton("Crear cuenta");
        btn->setMinimumHeight(32);
        connect(btn, &QPushButton::clicked, this, &LoginDialog::onRegister);
        form->addRow(btn);

        tabs->addTab(page, "Registrarse");
    }

    rootLayout->addWidget(tabs);

    // ── Etiqueta de estado ────────────────────
    m_statusLabel = new QLabel();
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet("color: #c0392b; font-weight: bold;");
    rootLayout->addWidget(m_statusLabel);
}

// ─────────────────────────────────────────────
//  Slots
// ─────────────────────────────────────────────

void LoginDialog::onLogin() {
    m_statusLabel->clear();
    QString user = m_loginUser->text().trimmed();
    QString pass = m_loginPass->text();

    if (user.isEmpty() || pass.isEmpty()) {
        m_statusLabel->setText("Completa usuario y contraseña.");
        return;
    }

    if (DatabaseManager::instance().loginUser(user, pass, m_user)) {
        accept();
    } else {
        m_statusLabel->setText("Usuario o contraseña incorrectos.");
        m_loginPass->clear();
    }
}

void LoginDialog::onRegister() {
    m_statusLabel->clear();
    QString fullName = m_regFullName->text().trimmed();
    QString user     = m_regUser->text().trimmed();
    QString pass     = m_regPass->text();

    if (fullName.isEmpty() || user.isEmpty() || pass.isEmpty()) {
        m_statusLabel->setText("Completa todos los campos.");
        return;
    }
    if (pass.length() < 4) {
        m_statusLabel->setText("La contraseña debe tener al menos 4 caracteres.");
        return;
    }
    if (user.contains(' ')) {
        m_statusLabel->setText("El nombre de usuario no puede tener espacios.");
        return;
    }

    bool ok = DatabaseManager::instance().registerUser(
        user, pass, fullName,
        m_regBirthYear->value(),
        m_regHeight->value(),
        m_regGender->currentText()
    );

    if (ok) {
        DatabaseManager::instance().loginUser(user, pass, m_user);
        QMessageBox::information(this, "¡Bienvenido/a!",
            QString("Cuenta creada exitosamente.\n¡Hola, %1!").arg(fullName));
        accept();
    } else {
        m_statusLabel->setText("El nombre de usuario ya existe.");
    }
}
