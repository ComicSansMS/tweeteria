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
#include <ui/users_list.hpp>

#include <QScrollBar>

#include <tweeteria/user.hpp>

#include <gbBase/Assert.hpp>
#include <gbBase/Log.hpp>

#include <algorithm>

UsersList::UsersList(QWidget* parent, DataModel& data_model)
    :QScrollArea(parent), m_list(new QWidget(this)), m_layout(QBoxLayout::TopToBottom),
     m_dataModel(&data_model)
{
    m_list->setLayout(&m_layout);
    setWidget(m_list);
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

UserWidget* UsersList::addUserWidget(tweeteria::User const& user)
{
    auto user_widget = new UserWidget(user, m_list);

    m_layout.addWidget(user_widget);
    m_elements.push_back(user_widget);

    connect(user_widget, &UserWidget::clicked, this, &UsersList::userClicked);
    return user_widget;
}

UserWidget* UsersList::getUserById(tweeteria::UserId user_id)
{
    auto it = std::find_if(begin(m_elements), end(m_elements), [user_id](UserWidget* u) { return u->getUserId() == user_id; });
    return (it != end(m_elements)) ? *it : nullptr;
}

void UsersList::userClicked(UserWidget* u)
{
    emit userSelected(u);
}

void UsersList::sortElements(UserSortOrder sorting)
{
    for(auto e : m_elements) { m_layout.removeWidget(e); }
    auto new_order = m_elements;
    switch(sorting) {
    case UserSortOrder::DateAdded: break;
    case UserSortOrder::Alphabetical:
        std::sort(begin(new_order), end(new_order),
                  [](UserWidget* lhs, UserWidget* rhs) { return lhs->getUserInfo().screen_name < rhs->getUserInfo().screen_name; });
        break;
    case UserSortOrder::Unread:
        std::sort(begin(new_order), end(new_order),
                  [](UserWidget* lhs, UserWidget* rhs) { return lhs->getUnreadCount() > rhs->getUnreadCount(); });
        break;
    }
    
    for(auto e : new_order) { m_layout.addWidget(e); }
}
