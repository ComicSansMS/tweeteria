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

#include <ui/tweets_list.hpp>

#include <gbBase/Assert.hpp>

TweetWidget::TweetWidget(tweeteria::Tweet const& t, tweeteria::User const& author, QWidget* parent)
    :QWidget(parent), m_tweet(t),
     m_avatar(new QLabel(this)), m_name(new QLabel(this)),
     m_twitterName(new QLabel(this)), m_text(new QLabel(this)), m_media(new QLabel(this)), m_date(new QLabel(this))
{
    GHULBUS_PRECONDITION_MESSAGE(t.user_id == author.id, "Author user must match tweet user.");
    m_layout.addLayout(&m_topRowLayout);

    {
        auto pal = palette();
        pal.setColor(QPalette::Background, Qt::white);
        setAutoFillBackground(true);
        setPalette(pal);
    }

    m_avatar->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    m_avatar->setMinimumSize(48, 48);
    m_topRowLayout.addWidget(m_avatar);

    m_topRowLayout.addLayout(&m_nameLayout);

    m_nameLayout.addStretch();
    m_name->setFont(QFont("Arial", 18, QFont::Bold));
    m_name->setText(QString::fromStdString(author.name));
    m_nameLayout.addWidget(m_name);

    m_twitterName->setText(QString("@") + QString::fromStdString(author.screen_name));
    m_twitterName->setFont(QFont("Arial", 12));
    m_twitterName->setStyleSheet("QLabel { color: grey; }");
    m_nameLayout.addWidget(m_twitterName);
    m_nameLayout.addStretch();

    m_layout.addStretch();
    m_text->setFont(QFont("Arial", 12));
    m_text->setText(QString::fromStdString(m_tweet.getDisplayText()));
    m_text->setWordWrap(true);
    m_text->setMinimumSize(512, m_text->height() * 3);
    m_text->setOpenExternalLinks(true);
    m_layout.addWidget(m_text);

    m_media->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    m_media->hide();
    m_layout.addWidget(m_media);

    m_layout.addStretch();
    m_date->setFont(QFont("Arial", 8));
    m_date->setStyleSheet("QLabel { color: grey; }");
    m_date->setText(QString::fromStdString(t.created_at));
    m_layout.addWidget(m_date);

    connect(this, &TweetWidget::imageArrived, this, &TweetWidget::onImageArrived, Qt::ConnectionType::QueuedConnection);
    connect(this, &TweetWidget::mediaArrived, this, &TweetWidget::onMediaArrived, Qt::ConnectionType::BlockingQueuedConnection);

    setMinimumWidth(590);

    m_layout.setContentsMargins(24, 24, 24, 24);

    setLayout(&m_layout);
}

void TweetWidget::onImageArrived(QPixmap p)
{
    m_avatar->setPixmap(p);
}

void TweetWidget::onMediaArrived(QPixmap p)
{
    QPixmap scaled = p.scaledToWidth(500, Qt::TransformationMode::SmoothTransformation);
    m_media->setPixmap(scaled);
    m_media->setMinimumSize(scaled.size());
    m_media->show();
}
