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
#include <ui/user_widget.hpp>

UserWidget::UserWidget(tweeteria::User const& u, QWidget* parent)
    :QWidget(parent), m_user(u),
     m_layout(QBoxLayout::Direction::LeftToRight),
     m_profileImage(new QLabel(this)),
     m_rightLayout(QBoxLayout::Direction::TopToBottom),
     m_userName(new QLabel(this)),
     m_twitterName(new QLabel(this)),
     m_description(new QLabel(this))
{
    m_layout.addWidget(m_profileImage);
    m_layout.addLayout(&m_rightLayout);
    m_rightLayout.addWidget(m_userName);
    m_rightLayout.addWidget(m_twitterName);
    m_rightLayout.addStretch(1);
    m_rightLayout.addWidget(m_description);
    m_rightLayout.addStretch(1);
    setLayout(&m_layout);

    m_profileImage->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    m_profileImage->setMinimumSize(73, 73);

    m_userName->setText(QString::fromUtf8(m_user.name.c_str()));
    m_userName->setFont(QFont("Arial", 18, QFont::Bold));
    m_twitterName->setText(QString("@") + QString::fromUtf8(m_user.screen_name.c_str()));
    m_twitterName->setFont(QFont("Arial", 12));
    m_twitterName->setStyleSheet("QLabel { color: grey; }");
    m_description->setText(QString::fromUtf8(m_user.description.c_str()));
    m_description->setFont(QFont("Arial", 12));
    m_description->setWordWrap(true);
    m_description->hide();
    //m_description->setOpenExternalLinks(true);
    //m_description->setText("Testtext <a href=\"http://www.google.de\">Link</a> More text.");

    connect(this, &UserWidget::imageArrived, this, &UserWidget::onImageArrived, Qt::QueuedConnection);
}

void UserWidget::onImageArrived(QPixmap const& image)
{
    m_profileImage->setPixmap(image);
}

