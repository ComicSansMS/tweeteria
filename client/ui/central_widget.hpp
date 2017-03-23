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
#ifndef TWEETERIA_CLIENT_INCLUDE_GUARD_UI_CENTRAL_WIDGET_HPP
#define TWEETERIA_CLIENT_INCLUDE_GUARD_UI_CENTRAL_WIDGET_HPP

#include <QWidget>

#include <QBoxLayout>
#include <QComboBox>
#include <QListWidget>
#include <QPushButton>

#include <user_sort_order.hpp>

#include <tweeteria/user.hpp>
#include <tweeteria/tweet.hpp>

#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace tweeteria {
class Tweeteria;
}

class DataModel;
class WebResourceProvider;
class ImageProvider;
class TweetsList;
class UserWidget;
class UsersList;

class CentralWidget : public QWidget
{
    Q_OBJECT
private:
    tweeteria::Tweeteria* m_tweeteria;
    DataModel* m_dataModel;
    std::unique_ptr<WebResourceProvider> m_webResourceProvider;
    std::unique_ptr<ImageProvider> m_imageProvider;

    QBoxLayout m_centralLayout;

    QVBoxLayout m_leftPaneLayout;
    QComboBox* m_sortingComboBox;
    UsersList* m_usersList;

    QBoxLayout m_rightPaneLayout;
    TweetsList* m_tweetsList;

    QBoxLayout m_buttonsLayout;
    QPushButton* m_nextPage;

    std::vector<UserWidget*> m_usersInList;
    std::vector<tweeteria::TweetId> m_tweets;

    tweeteria::UserId m_selectedUser;
public:
    CentralWidget(tweeteria::Tweeteria& tweeteria, DataModel& data_model, QWidget* parent);
    ~CentralWidget();

signals:
    void tweetMarkedAsRead(tweeteria::TweetId tweet_id, tweeteria::UserId author_id);
    void userSelectionChanged(tweeteria::UserId selected_user);

    void additionalTimelineTweetsRequest(tweeteria::UserId, tweeteria::TweetId current_max_id);

public slots:
    void userSelected(UserWidget* user_widget);
    void onUserInfoUpdate(tweeteria::UserId updated_user_id, bool is_friend);
    void onUserTimelineUpdate(tweeteria::UserId updated_user_id);
    void onNextPageClicked();
    void onUnreadForUserChanged(tweeteria::UserId user_id, int unread_count);
private slots:
    void markTweetAsRead(tweeteria::TweetId tweet_id, tweeteria::UserId author_id);
    void onSortingChanged(UserSortOrder sorting);
};

#endif
