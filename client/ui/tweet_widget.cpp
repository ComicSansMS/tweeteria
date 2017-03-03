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
#include <ui/tweet_widget.hpp>

TweetWidget::TweetWidget(tweeteria::Tweet const& t, QWidget* parent)
    :QWidget(parent), m_tweet(t),
     m_layout(new QBoxLayout(QBoxLayout::TopToBottom, this)), m_topRowLayout(new QBoxLayout(QBoxLayout::LeftToRight, parent)),
     m_avatar(new QLabel(this)), m_nameLayout(new QBoxLayout(QBoxLayout::TopToBottom, parent)), m_name(new QLabel(this)),
     m_twitterName(new QLabel(this)), m_text(new QLabel(this)), m_date(new QLabel(this))
{
    m_layout->addLayout(m_topRowLayout);

    m_avatar->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    m_avatar->setStyleSheet("QLabel { background-color: #FF8080 }");
    m_avatar->setMinimumSize(48, 48);
    m_topRowLayout->addWidget(m_avatar);

    m_topRowLayout->addLayout(m_nameLayout);

    m_nameLayout->addStretch();
    m_name->setFont(QFont("Arial", 18, QFont::Bold));
    m_name->setText("User Name");
    m_nameLayout->addWidget(m_name);

    m_twitterName->setText(QString("@") + QString::fromUtf8("user_name"));
    m_twitterName->setFont(QFont("Arial", 12));
    m_twitterName->setStyleSheet("QLabel { color: grey; }");
    m_nameLayout->addWidget(m_twitterName);
    m_nameLayout->addStretch();

    m_layout->addStretch();
    m_text->setFont(QFont("Arial", 12));
    m_text->setText(QString::fromStdString(m_tweet.text));
    m_layout->addWidget(m_text);

    m_layout->addStretch();
    m_date->setFont(QFont("Arial", 8));
    m_date->setStyleSheet("QLabel { color: grey; }");
    m_date->setText("3:27 AM - 3 May 2016");
    m_layout->addWidget(m_date);

    setMinimumWidth(590);
}

