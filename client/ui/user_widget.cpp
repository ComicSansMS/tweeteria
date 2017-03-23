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

#include <QMouseEvent>

#include <gbBase/Log.hpp>

UserWidget::UserWidget(tweeteria::User const& u, QWidget* parent)
    :QWidget(parent), m_user(u),
     m_layout(QBoxLayout::Direction::LeftToRight),
     m_profileImage(new QLabel(this)),
     m_rightLayout(QBoxLayout::Direction::TopToBottom),
     m_userName(new QLabel(this)),
     m_twitterName(new QLabel(this)),
     m_unread(new QLabel(this)),
     m_description(new QLabel(this)),
     m_unreadCount(-1)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setAutoFillBackground(true);
    setPalette(pal);

    m_layout.addWidget(m_profileImage);
    m_layout.addLayout(&m_rightLayout);
    m_rightLayout.addWidget(m_userName);
    m_rightLayout.addWidget(m_twitterName);
    m_rightLayout.addStretch(1);
    m_rightLayout.addWidget(m_description);
    m_rightLayout.addStretch(1);
    m_layout.addStretch();
    m_layout.addWidget(m_unread);
    m_layout.addSpacing(10);
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

    m_unread->setFont(QFont("Arial", 18, QFont::Bold));
    m_unread->setStyleSheet("QLabel { color: #334455; }");
    m_unread->hide();
    //m_description->setOpenExternalLinks(true);
    //m_description->setText("Testtext <a href=\"http://www.google.de\">Link</a> More text.");

    connect(this, &UserWidget::imageArrived, this, &UserWidget::onImageArrived, Qt::QueuedConnection);
    connect(this, &UserWidget::unreadUpdate, this, &UserWidget::onUnreadUpdated, Qt::QueuedConnection);
}

UserWidget::~UserWidget()
{
    GHULBUS_LOG(Trace, "Destroyed widget for user " << m_user.id.id << " (@" << m_user.screen_name << ").");
}

void UserWidget::onImageArrived(QPixmap const& image)
{
    m_profileImage->setPixmap(image);
}

void UserWidget::onUnreadUpdated(int unread)
{
    if(unread < 0) {
        m_unread->setText(QString("(??)"));
        m_unread->show();
    } else if(unread == 0) {
        m_unread->hide();
    } else if(unread >= 50) {
        m_unread->setText(QString("(50+)"));
        m_unread->show();
    } else {
        m_unread->setText(QString("(") + QString::number(unread) + QString(")"));
        m_unread->show();
    }
    m_unreadCount = unread;
}

int UserWidget::getUnreadCount() const
{
    return m_unreadCount;
}

tweeteria::UserId UserWidget::getUserId() const
{
    return m_user.id;
}

tweeteria::User const& UserWidget::getUserInfo() const
{
    return m_user;
}

void UserWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked(this);
    }
}

void UserWidget::enterEvent(QEvent* event)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Window, pal.color(QPalette::Highlight));
    setAutoFillBackground(true);
    setPalette(pal);
}

void UserWidget::leaveEvent(QEvent* event)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setAutoFillBackground(true);
    setPalette(pal);
}
