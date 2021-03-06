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
#ifndef TWEETERIA_CLIENT_INCLUDE_GUARD_UI_OPENING_DIALOG_HPP
#define TWEETERIA_CLIENT_INCLUDE_GUARD_UI_OPENING_DIALOG_HPP

#include <qt_begin_disable_warnings.hpp>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include <QSpacerItem>
#include <QStackedWidget>
#include <QTimer>
#include <qt_end_disable_warnings.hpp>

#include <tweeteria/proxy_config.hpp>
#include <tweeteria/user.hpp>

class SvgIcon;

class OpeningDialog : public QWidget {
    Q_OBJECT
private:
    QVBoxLayout m_outerLayout;
    QHBoxLayout m_closeButtonLayout;
    QPushButton* m_closeButton;

    QLabel* m_welcomeText;
    QHBoxLayout m_logoLayout;
    SvgIcon* m_logoIcon;
    QLabel* m_tweeteriaText;
    QPushButton* m_configureProxyButton;
    QPushButton* m_goButton;

    QHBoxLayout m_statusLayout;
    SvgIcon* m_statusWaitIcon;
    QLabel* m_statusLabel;

    QTimer* m_connectivityTimeout;

    QLabel* m_authenticateUrl;
    QLineEdit* m_authenticatePin;
    QPushButton* m_authenticateButton;

    tweeteria::ProxyConfig m_proxyConfig;
    int m_connectivityTestGenerationCount;
public:
    OpeningDialog();

signals:
    void go();
    void proxyConfigurationChanged(tweeteria::ProxyConfig new_proxy_config);
    void oauthPinEntered(QString const& pin_str);
public slots:
    void hideStatus();
    void showStatus(QString const& text, bool isInProgress);

    void onStartConnectivityTest(int generation_count);
    void onConnectivityTestSuccessful(int generation_count);
    void onConnectivityTestFailed(int generation_count, QString const& error);
    void onConnectivityTestTimeout();

    void onOAuthUrlArrived(QString url);

    void onCredentialsVerified(tweeteria::User const& user);

private slots:
    void onCloseButtonClicked();
    void onConfigureProxyClicked();
};

#endif
