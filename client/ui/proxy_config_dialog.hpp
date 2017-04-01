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
#ifndef TWEETERIA_CLIENT_INCLUDE_GUARD_UI_PROXY_CONFIG_DIALOG_HPP
#define TWEETERIA_CLIENT_INCLUDE_GUARD_UI_PROXY_CONFIG_DIALOG_HPP

#include <qt_begin_disable_warnings.hpp>
#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include <QStackedWidget>
#include <qt_end_disable_warnings.hpp>

class SvgIcon;

class ProxyConfigDialog : public QWidget {
    Q_OBJECT
public:
    enum class ProxyMode {
        None = 0,
        Auto,
        System,
        Manual
    };
private:
    QGridLayout m_outerLayout;
    QLabel* m_headerLabel;
    QComboBox* m_comboBox;

    QLabel* m_addressLabel;
    QLineEdit* m_addressEdit;

    QLabel* m_portLabel;
    QLineEdit* m_portEdit;

    QFormLayout m_credentialsLayout;
    QCheckBox* m_credentialsCheckbox;
    QLineEdit* m_credentialsUserEdit;
    QLineEdit* m_credentialsPassEdit;
    QCheckBox* m_displayPasswordCheckBox;

    QDialogButtonBox* m_dialogButtons;
public:
    ProxyConfigDialog(QWidget* parent);

signals:
    void accepted();
    void rejected();
private slots:
    void updateCredentialsWidgets(int check_state);
private:
    void proxyModeChange(ProxyMode const& mode);
};

#endif
