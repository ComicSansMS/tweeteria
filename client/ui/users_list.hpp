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
#ifndef TWEETERIA_CLIENT_INCLUDE_GUARD_UI_USERS_LIST_HPP
#define TWEETERIA_CLIENT_INCLUDE_GUARD_UI_USERS_LIST_HPP

#include <ui/user_widget.hpp>

#include <user_sort_order.hpp>

#include <QBoxLayout>
#include <QScrollArea>
#include <QWidget>

#include <vector>

namespace tweeteria {
struct Tweet;
struct User;
}

class ImageProvider;
class DataModel;

class UsersList : public QScrollArea
{
    Q_OBJECT
private:
    QWidget* m_list;
    QBoxLayout m_layout;
    std::vector<UserWidget*> m_elements;

    DataModel* m_dataModel;
    ImageProvider* m_imageProvider;
public:
    UsersList(QWidget* parent, DataModel& data_model);

    UserWidget* addUserWidget(tweeteria::User const& user);

    UserWidget* getUserById(tweeteria::UserId user_id);

signals:
    void userSelected(UserWidget* u);

public slots:
    void sortElements(UserSortOrder sorting);

private slots:
    void userClicked(UserWidget* u);
};

#endif
