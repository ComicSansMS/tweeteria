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
#ifndef TWEETERIA_CLIENT_INCLUDE_GUARD_UI_USER_WIDGET_HPP
#define TWEETERIA_CLIENT_INCLUDE_GUARD_UI_USER_WIDGET_HPP

#include <tweeteria/user.hpp>

#include <QBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QWidget>

class UserWidget : public QWidget
{
    Q_OBJECT
private:
    tweeteria::User m_user;
    QBoxLayout m_layout;
    QLabel* m_profileImage;
    QBoxLayout m_rightLayout;
    QLabel* m_userName;
    QLabel* m_twitterName;
    QLabel* m_unread;
    QLabel* m_description;
public:
    UserWidget(tweeteria::User const& u, QWidget* parent);
public slots:
    void onImageArrived(QPixmap const& image);
    void onUnreadUpdated(int unread);
signals:
    void imageArrived(QPixmap const& image);
    void unreadUpdate(int unread);
};

#endif
