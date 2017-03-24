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
#include <QMenu>
#include <QPixmap>
#include <QPushButton>
#include <QSvgWidget>
#include <QWidget>

class DataModel;

class TweetWidget : public QWidget
{
    Q_OBJECT
private:
    tweeteria::Tweet m_tweet;
    tweeteria::User m_author;
    DataModel* m_dataModel;
    QVBoxLayout m_layout;
    QHBoxLayout m_topRowLayout;
    QLabel* m_avatar;
    QVBoxLayout m_nameLayout;
    QLabel* m_name;
    QLabel* m_twitterName;
    QPushButton* m_menuButton;
    QLabel* m_header;
    QLabel* m_text;
    QLabel* m_media;
    QLabel* m_date;

    QHBoxLayout m_actionsLayout;
    class SvgIcon : public QSvgWidget {
    public:
        SvgIcon(QWidget* parent)
            :QSvgWidget(parent)
        {}

        QSize sizeHint() const override
        {
            return QSize(16, 16);
        }
    };
    SvgIcon* m_replies;
    QLabel* m_repliesLabel;

    class Menu {
    public:
        QMenu* menu;
        QAction* markAsRead;
        QAction* copyUrl;
        QAction* copyJSON;
        QAction* openInBrowser;

        Menu(TweetWidget* parent);
    };
    Menu* m_menu;
public:
    TweetWidget(tweeteria::Tweet const& t, DataModel& data_model, QWidget* parent=nullptr);

    tweeteria::Tweet const& getDisplayedTweet() const;
    tweeteria::UserId getDisplayedAuthorId() const;
signals:
    void imageArrived(QPixmap p);
    void mediaArrived(QPixmap p);
    void markedAsRead(tweeteria::TweetId, tweeteria::UserId);
private slots:
    void onImageArrived(QPixmap p);
    void onMediaArrived(QPixmap p);
    void openMenu();
    void markAsRead();
    void copyUrl();
    void copyJSON();
    void openInBrowser();
};

#endif
