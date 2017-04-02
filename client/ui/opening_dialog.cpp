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
#include <ui/opening_dialog.hpp>
#include <ui/proxy_config_dialog.hpp>

#include <ui/svg_icon.hpp>

#include <gbBase/Log.hpp>

OpeningDialog::OpeningDialog()
    :QWidget(nullptr, Qt::FramelessWindowHint), m_closeButton(new QPushButton(this)),
     m_welcomeText(new QLabel(this)), m_logoIcon(new SvgIcon(this)),
     m_tweeteriaText(new QLabel(this)), m_configureProxyButton(new QPushButton(this)),
     m_goButton(new QPushButton(this)), m_statusWaitIcon(new SvgIcon(this)), m_statusLabel(new QLabel(this)),
     m_connectivityTimeout(new QTimer(this)), m_connectivityTestGenerationCount(0)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    resize(400, 400);

    m_closeButton->setText("X");
    m_closeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_closeButtonLayout.addStretch();
    m_closeButtonLayout.addWidget(m_closeButton);
    m_outerLayout.addLayout(&m_closeButtonLayout);
    m_outerLayout.addStretch();

    m_welcomeText->setText("Welcome to");
    m_welcomeText->setAlignment(Qt::AlignCenter);
    m_welcomeText->setFont(QFont("Arial", 32, QFont::Bold));
    m_welcomeText->setStyleSheet("QLabel { color: #1DA1F2; }");
    m_outerLayout.addWidget(m_welcomeText);

    m_logoLayout.addStretch();
    m_logoIcon->load(QString(":/logo.svg"));
    m_logoIcon->setIconSize(QSize(90,64));
    m_logoIcon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_logoLayout.addWidget(m_logoIcon);
    m_tweeteriaText->setFont(QFont("Arial", 32, QFont::Bold));
    m_tweeteriaText->setText("Tweeteria");
    m_tweeteriaText->setStyleSheet("QLabel { color: #1DA1F2; }");
    m_logoLayout.addWidget(m_tweeteriaText);
    m_logoLayout.addStretch();
    m_outerLayout.addLayout(&m_logoLayout);
    m_outerLayout.addStretch();

    m_outerLayout.addLayout(&m_statusLayout);
    m_statusLayout.setAlignment(Qt::AlignHCenter);
    m_statusWaitIcon->load(QString(":/loading_icon.svg"));
    m_statusLayout.addWidget(m_statusWaitIcon);

    m_statusLabel->setText("");
    m_statusLayout.addWidget(m_statusLabel);

    // insert a spacer so that the status label position
    //  does not change when we hide the status icon.
    m_statusLayout.addSpacerItem(new QSpacerItem(0, m_statusWaitIcon->sizeHint().height()));

    m_configureProxyButton->setText("Configure Proxy");
    m_configureProxyButton->hide();
    m_outerLayout.addWidget(m_configureProxyButton);

    m_goButton->setText("Let's go...");
    m_goButton->hide();
    m_outerLayout.addWidget(m_goButton);

    connect(m_goButton, &QPushButton::clicked, this, [this]() { emit go(); });

    setLayout(&m_outerLayout);

    m_connectivityTimeout->setSingleShot(true);

    connect(m_closeButton, &QPushButton::clicked, this, &OpeningDialog::onCloseButtonClicked);
    connect(m_configureProxyButton, &QPushButton::clicked, this, &OpeningDialog::onConfigureProxyClicked);
    connect(m_connectivityTimeout, &QTimer::timeout, this, &OpeningDialog::onConnectivityTestTimeout);
}

void OpeningDialog::onCloseButtonClicked()
{
    GHULBUS_LOG(Trace, "Close clicked");
    close();
}

void OpeningDialog::onConfigureProxyClicked()
{
    ProxyConfigDialog dialog(this);
    dialog.setFromProxyConfig(m_proxyConfig);
    dialog.setWindowModality(Qt::WindowModal);
    hide();
    auto const res = dialog.exec();
    show();
    if(res == QDialog::Accepted) {
        m_proxyConfig = dialog.getProxyConfig();
        emit proxyConfigurationChanged(m_proxyConfig);
    }
}

void OpeningDialog::hideStatus()
{
    m_statusWaitIcon->hide();
    m_statusLabel->hide();
}

void OpeningDialog::showStatus(QString const& text, bool isInProgress)
{
    m_statusLabel->setText(text);
    m_statusLabel->show();
    if(isInProgress) {
        m_statusWaitIcon->show();
    } else {
        m_statusWaitIcon->hide();
    }
}

void OpeningDialog::onStartConnectivityTest(int generation_count)
{
    showStatus("Checking Internet connection...", true);
    // timeout is 5 seconds
    m_connectivityTimeout->start(5000);
    m_connectivityTestGenerationCount = generation_count;
}

void OpeningDialog::onConnectivityTestSuccessful(int generation_count)
{
    if(m_connectivityTestGenerationCount == generation_count)
    {
        m_connectivityTimeout->stop();
        showStatus("Connection successful.", false);
        m_goButton->show();
        m_configureProxyButton->hide();
        m_connectivityTestGenerationCount = 0;
    }
}

void OpeningDialog::onConnectivityTestFailed(int generation_count, QString const& error)
{
    if(m_connectivityTestGenerationCount == generation_count)
    {
        m_connectivityTimeout->stop();
        showStatus("Connection failed: " + error, false);
        m_configureProxyButton->show();
        m_connectivityTestGenerationCount = 0;
    }
}

void OpeningDialog::onConnectivityTestTimeout()
{
    m_configureProxyButton->show();
}
