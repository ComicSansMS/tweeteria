/*
 * Tweeteria - A minimalistic tweet reader.
 * Copyright (C) 2017  Andreas Weis (der_ghulbus@ghulbus-inc.de)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <ui/proxy_config_dialog.hpp>

#include <QIntValidator>
#include <QRegExpValidator>

#include <gbBase/Log.hpp>

ProxyConfigDialog::ProxyConfigDialog(QWidget* parent)
    :QDialog(parent,  Qt::WindowTitleHint | Qt::WindowSystemMenuHint), m_headerLabel(new QLabel(this)), m_comboBox(new QComboBox(this)),
     m_addressLabel(new QLabel(this)), m_addressEdit(new QLineEdit(this)), m_portLabel(new QLabel(this)), m_portEdit(new QLineEdit(this)),
     m_credentialsCheckbox(new QCheckBox(this)), m_credentialsUserEdit(new QLineEdit(this)),
     m_credentialsPassEdit(new QLineEdit(this)), m_displayPasswordCheckBox(new QCheckBox(this)),
     m_dialogButtons(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this))
{
    setWindowTitle("Proxy Configuration");
    setMinimumSize(400, 300);
    setLayout(&m_outerLayout);
    // proxy address column should stretch more than the others
    m_outerLayout.setColumnStretch(1, 1);

    m_headerLabel->setText("If you access the Internet through a proxy, configure it here.");
    m_outerLayout.addWidget(m_headerLabel, 0, 0, 1, 4);

    m_comboBox->addItem("No proxy");
    m_comboBox->addItem("Auto-detect proxy settings");
    m_comboBox->addItem("Use system proxy settings");
    m_comboBox->addItem("Manual proxy configuration");
    m_outerLayout.addWidget(m_comboBox, 1, 0, 1, 4);

    m_outerLayout.addItem(new QSpacerItem(0, 15), m_outerLayout.rowCount(), 0);

    m_addressLabel->setText("Proxy Address: ");
    auto const addr_port_row = m_outerLayout.rowCount();
    m_outerLayout.addWidget(m_addressLabel, addr_port_row, 0);
    m_addressEdit->setValidator(new QRegExpValidator(QRegExp("\\S+"), m_addressEdit));
    m_outerLayout.addWidget(m_addressEdit, addr_port_row, 1);

    m_portLabel->setText("Port: ");
    m_outerLayout.addWidget(m_portLabel, addr_port_row, 2);
    m_portEdit->setValidator(new QIntValidator(0, 65535, m_portEdit));
    m_outerLayout.addWidget(m_portEdit, addr_port_row, 3);

    m_outerLayout.addItem(new QSpacerItem(0, 10), m_outerLayout.rowCount(), 0);

    m_outerLayout.addLayout(&m_credentialsLayout, m_outerLayout.rowCount(), 1, 1, 3);
    m_credentialsCheckbox->setText("Proxy requires authentication");
    m_credentialsLayout.addRow(m_credentialsCheckbox);
    m_credentialsLayout.addRow("Login: ", m_credentialsUserEdit);

    m_credentialsPassEdit->setEchoMode(QLineEdit::Password);
    m_credentialsLayout.addRow("Password: ", m_credentialsPassEdit);

    m_displayPasswordCheckBox->setText("Display password");
    m_credentialsLayout.addRow("", m_displayPasswordCheckBox);
    m_outerLayout.setRowStretch(m_outerLayout.rowCount(), 2);

    m_outerLayout.addWidget(m_dialogButtons, m_outerLayout.rowCount(), 0, 1, 4);

    updateCredentialsWidgets(m_credentialsCheckbox->checkState());
    proxyModeChange(static_cast<ProxyMode>(m_comboBox->currentIndex()));

    connect(m_credentialsCheckbox, &QCheckBox::stateChanged, this, &ProxyConfigDialog::updateCredentialsWidgets);
    connect(m_displayPasswordCheckBox, &QCheckBox::stateChanged,
            this, [this](int check_state) {
                m_credentialsPassEdit->setEchoMode((check_state == Qt::Checked) ? QLineEdit::Normal : QLineEdit::Password);
            });
    connect(m_comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [this](int index) { proxyModeChange(static_cast<ProxyMode>(index)); });
    connect(m_dialogButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_dialogButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void ProxyConfigDialog::updateCredentialsWidgets(int check_state)
{
    auto const do_enable = (check_state == Qt::Checked);
    m_credentialsUserEdit->setEnabled(do_enable);
    m_credentialsLayout.labelForField(m_credentialsUserEdit)->setEnabled(do_enable);
    m_credentialsPassEdit->setEnabled(do_enable);
    m_credentialsLayout.labelForField(m_credentialsPassEdit)->setEnabled(do_enable);
    m_displayPasswordCheckBox->setEnabled(do_enable);
    if(!do_enable) { m_displayPasswordCheckBox->setChecked(false); }
}

void ProxyConfigDialog::proxyModeChange(ProxyMode const& mode)
{
    if(mode == ProxyMode::Manual) {
        m_addressLabel->show();
        m_addressEdit->show();
        m_portLabel->show();
        m_portEdit->show();
        m_credentialsCheckbox->show();
        m_credentialsUserEdit->show();
        m_credentialsLayout.labelForField(m_credentialsUserEdit)->show();
        m_credentialsPassEdit->show();
        m_credentialsLayout.labelForField(m_credentialsPassEdit)->show();
        m_displayPasswordCheckBox->show();
    } else {
        m_addressLabel->hide();
        m_addressEdit->hide();
        m_portLabel->hide();
        m_portEdit->hide();
        m_credentialsCheckbox->hide();
        m_credentialsUserEdit->hide();
        m_credentialsLayout.labelForField(m_credentialsUserEdit)->hide();
        m_credentialsPassEdit->hide();
        m_credentialsLayout.labelForField(m_credentialsPassEdit)->hide();
        m_displayPasswordCheckBox->hide();
        m_displayPasswordCheckBox->setChecked(false);
    }
}

void ProxyConfigDialog::setFromProxyConfig(tweeteria::ProxyConfig const& proxy_config)
{
    switch(proxy_config.mode) {
    case tweeteria::ProxyConfig::Mode::None:
        m_comboBox->setCurrentIndex(static_cast<int>(ProxyMode::None));
        break;
    case tweeteria::ProxyConfig::Mode::Auto:
        m_comboBox->setCurrentIndex(static_cast<int>(ProxyMode::Auto));
        break;
    case tweeteria::ProxyConfig::Mode::System:
        m_comboBox->setCurrentIndex(static_cast<int>(ProxyMode::System));
        break;
    case tweeteria::ProxyConfig::Mode::Manual:
        m_comboBox->setCurrentIndex(static_cast<int>(ProxyMode::Manual));
        m_addressEdit->setText(QString::fromStdString(proxy_config.proxy_url));
        m_portEdit->setText(QString::number(proxy_config.proxy_port));
        if(!proxy_config.proxy_login_user.empty() || !proxy_config.proxy_login_password.empty()) {
            m_credentialsCheckbox->setChecked(true);
            m_credentialsUserEdit->setText(QString::fromStdString(proxy_config.proxy_login_user));
            m_credentialsPassEdit->setText(QString::fromStdString(proxy_config.proxy_login_password));
        }
        break;
    default:
        GHULBUS_LOG(Error, "Invalid mode in proxy config: " << static_cast<int>(proxy_config.mode) << ".");
        break;
    }
}

tweeteria::ProxyConfig ProxyConfigDialog::getProxyConfig() const
{
    tweeteria::ProxyConfig ret;
    switch(static_cast<ProxyMode>(m_comboBox->currentIndex())) {
    case ProxyMode::None:
        ret.mode = tweeteria::ProxyConfig::Mode::None;
        break;
    case ProxyMode::Auto:
        ret.mode = tweeteria::ProxyConfig::Mode::Auto;
        break;
    case ProxyMode::System:
        ret.mode = tweeteria::ProxyConfig::Mode::System;
        break;
    case ProxyMode::Manual:
        ret.mode = tweeteria::ProxyConfig::Mode::Manual;
        break;
    default:
        GHULBUS_LOG(Error, "Invalid option in proxy dialog: " << m_comboBox->currentIndex() << ".");
        ret.mode = tweeteria::ProxyConfig::Mode::None;
    }
    ret.proxy_url = m_addressEdit->text().toStdString();
    ret.proxy_port = m_portEdit->text().toUShort();
    if(m_credentialsCheckbox->isChecked()) {
        ret.proxy_login_user = m_credentialsUserEdit->text().toStdString();
        ret.proxy_login_password = m_credentialsPassEdit->text().toStdString();
    }
    return ret;
}
