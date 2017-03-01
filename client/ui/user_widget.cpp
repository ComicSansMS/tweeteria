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
     m_layout(new QBoxLayout(QBoxLayout::Direction::LeftToRight, this)),
     m_profileImage(new QLabel(this)),
     m_rightLayout(new QBoxLayout(QBoxLayout::Direction::TopToBottom, this)),
     m_userName(new QLabel(this)),
     m_twitterName(new QLabel(this)),
     m_description(new QLabel(this))
{
    m_layout->addWidget(m_profileImage);
    m_layout->addLayout(m_rightLayout);
    m_rightLayout->addWidget(m_userName);
    m_rightLayout->addWidget(m_twitterName);
    m_rightLayout->addStretch(1);
    m_rightLayout->addWidget(m_description);
    m_rightLayout->addStretch(1);

    m_profileImage->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    m_profileImage->setStyleSheet("QLabel { background-color: #FF8080 }");
    m_profileImage->setMinimumSize(200, 200);

    m_userName->setText(QString::fromUtf8(m_user.name.c_str()));
    m_userName->setFont(QFont("Arial", 18, QFont::Bold));
    m_twitterName->setText(QString("@") + QString::fromUtf8(m_user.screen_name.c_str()));
    m_twitterName->setFont(QFont("Arial", 12));
    m_twitterName->setStyleSheet("QLabel { color: grey; }");
    m_description->setText(QString::fromUtf8(m_user.description.c_str()));
    m_description->setFont(QFont("Arial", 12));
    m_description->setWordWrap(true);
    //m_description->setOpenExternalLinks(true);
    //m_description->setText("Testtext <a href=\"http://www.google.de\">Link</a> More text.");
}

void UserWidget::imageArrived(QPixmap const& image)
{
    m_profileImage->setPixmap(image);
}

