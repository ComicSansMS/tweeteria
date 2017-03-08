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
#ifndef TWEETERIA_CLIENT_INCLUDE_GUARD_UI_TWEET_WIDGET_HPP
#define TWEETERIA_CLIENT_INCLUDE_GUARD_UI_TWEET_WIDGET_HPP

#include <tweeteria/tweet.hpp>
#include <tweeteria/user.hpp>

#include <QBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QWidget>

class TweetWidget : public QWidget
{
    Q_OBJECT
private:
    tweeteria::Tweet m_tweet;
    QBoxLayout* m_layout;
    QBoxLayout* m_topRowLayout;
    QLabel* m_avatar;
    QBoxLayout* m_nameLayout;
    QLabel* m_name;
    QLabel* m_twitterName;
    QLabel* m_text;
    QLabel* m_media;
    QLabel* m_date;
public:
    TweetWidget(tweeteria::Tweet const& t, tweeteria::User const& author, QWidget* parent);

signals:
    void imageArrived(QPixmap p);
    void mediaArrived(QPixmap p);
private slots:
    void onImageArrived(QPixmap p);
    void onMediaArrived(QPixmap p);
};

#endif
