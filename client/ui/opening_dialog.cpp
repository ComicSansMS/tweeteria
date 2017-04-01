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

#include <ui/svg_icon.hpp>

#include <gbBase/Log.hpp>

OpeningDialog::OpeningDialog()
    :QWidget(nullptr, Qt::FramelessWindowHint), m_closeButton(new QPushButton(this)),
     m_welcomeText(new QLabel(this)), m_logoIcon(new SvgIcon(this)),
     m_tweeteriaText(new QLabel(this)), m_startButton(new QPushButton(this)), m_waitIcon(new SvgIcon(this))
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

    m_waitIcon->load(QString(":/loading_icon.svg"));
    m_waitIcon->setIconSize(QSize(m_startButton->size().height(), m_startButton->size().height()));
    m_outerLayout.addWidget(m_waitIcon);

    m_startButton->setText("Let's go...");
    m_outerLayout.addWidget(m_startButton);

    connect(m_startButton, &QPushButton::clicked, this, [this]() { emit go(); });

    setLayout(&m_outerLayout);

    connect(m_closeButton, &QPushButton::clicked, this, &OpeningDialog::onCloseButtonClicked);
}

void OpeningDialog::onCloseButtonClicked()
{
    GHULBUS_LOG(Trace, "Close clicked");
    close();
}
