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

#include <ui/data_model.hpp>
#include <ui/tweets_list.hpp>

#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QUrl>

#include <gbBase/Assert.hpp>
#include <gbBase/Log.hpp>

namespace {
class SvgIconReplySource {
private:
    static constexpr char const* preamble() {
        return R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 65 72"> <path )";
    }

    static constexpr char const* body() {
        return R"( d="M41 31h-9V19c0-1.14-.647-2.183-1.668-2.688-1.022-.507-2.243-.39-3.15.302l-21 16C5.438 33.18 5 34.064 5 35s.437 1.82 1.182 2.387l21 16c.533.405 1.174.613 1.82.613.453 0 .908-.103 1.33-.312C31.354 53.183 32 52.14 32 51V39h9c5.514 0 10 4.486 10 10 0 2.21 1.79 4 4 4s4-1.79 4-4c0-9.925-8.075-18-18-18z"/> </svg>)";
    }
public:

    enum class PressedState {
        Default,
        Pressed
    };

    static std::string getIconSource(PressedState pressed_state)
    {
        std::string ret;
        if(pressed_state == PressedState::Pressed) {
            return std::string(preamble()) + std::string(R"(fill="#AAB8C2" opacity="0.5")") + std::string(body());
        } else {
            return std::string(preamble()) + std::string(R"(fill="#AAB8C2")") + std::string(body());
        }
    }
};

class SvgIconRetweetSource {
private:
    static constexpr char const* preamble() {
        return R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 75 72"> <path )";
    }

    static constexpr char const* body() {
        return R"( d="M70.676 36.644C70.166 35.636 69.13 35 68 35h-7V19c0-2.21-1.79-4-4-4H34c-2.21 0-4 1.79-4 4s1.79 4 4 4h18c.552 0 .998.446 1 .998V35h-7c-1.13 0-2.165.636-2.676 1.644-.51 1.01-.412 2.22.257 3.13l11 15C55.148 55.545 56.046 56 57 56s1.855-.455 2.42-1.226l11-15c.668-.912.767-2.122.256-3.13zM40 48H22c-.54 0-.97-.427-.992-.96L21 36h7c1.13 0 2.166-.636 2.677-1.644.51-1.01.412-2.22-.257-3.13l-11-15C18.854 15.455 17.956 15 17 15s-1.854.455-2.42 1.226l-11 15c-.667.912-.767 2.122-.255 3.13C3.835 35.365 4.87 36 6 36h7l.012 16.003c.002 2.208 1.792 3.997 4 3.997h22.99c2.208 0 4-1.79 4-4s-1.792-4-4-4z"/> </svg>)";
    }
public:

    enum class ActiveState {
        Inactive,
        Active
    };

    static std::string getIconSource(ActiveState active_state)
    {
        std::string ret;
        if(active_state == ActiveState::Active) {
            return std::string(preamble()) + std::string(R"(fill="#19CF86")") + std::string(body());
        } else {
            return std::string(preamble()) + std::string(R"(fill="#AAB8C2")") + std::string(body());
        }
    }
};

class SvgIconLikeSource {
private:
    static constexpr char const* preamble() {
        return R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 54 72"> <path )";
    }

    static constexpr char const* body() {
        return R"( d="M38.723,12c-7.187,0-11.16,7.306-11.723,8.131C26.437,19.306,22.504,12,15.277,12C8.791,12,3.533,18.163,3.533,24.647 C3.533,39.964,21.891,55.907,27,56c5.109-0.093,23.467-16.036,23.467-31.353C50.467,18.163,45.209,12,38.723,12z"/> </svg>)";
    }
public:

    enum class ActiveState {
        Inactive,
        Active
    };

    static std::string getIconSource(ActiveState pressed_state)
    {
        std::string ret;
        if(pressed_state == ActiveState::Active) {
            return std::string(preamble()) + std::string(R"(fill="#E81C4F")") + std::string(body());
        } else {
            return std::string(preamble()) + std::string(R"(fill="#AAB8C2")") + std::string(body());
        }
    }
};
}

TweetWidget::TweetWidget(tweeteria::Tweet const& t, DataModel& data_model, QWidget* parent)
    :QWidget(parent), m_tweet(t), m_author(*data_model.getUser(t.user_id)), m_dataModel(&data_model),
     m_avatar(new QLabel(this)), m_name(new QLabel(this)), m_twitterName(new QLabel(this)),
     m_menuButton(new QPushButton(this)), m_header(new QLabel(this)), m_text(new QLabel(this)), m_media(new QLabel(this)),
     m_date(new QLabel(this)), m_replies(new SvgIcon(this)), m_repliesLabel(new QLabel(this)),
     m_retweeets(new SvgIcon(this)), m_retweetsLabel(new QLabel(this)),
     m_favorites(new SvgIcon(this)), m_favoritesLabel(new QLabel(this)), m_menu(new Menu(this))
{
    m_layout.addLayout(&m_topRowLayout);

    {
        auto pal = palette();
        pal.setColor(QPalette::Window, Qt::white);
        setAutoFillBackground(true);
        setPalette(pal);
    }

    m_avatar->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    m_avatar->setMinimumSize(48, 48);
    m_topRowLayout.addWidget(m_avatar);

    m_topRowLayout.addLayout(&m_nameLayout);

    tweeteria::Tweet const& displayed_tweet = getDisplayedTweet();
    auto displayed_author = m_dataModel->getUser(getDisplayedAuthorId());
    m_nameLayout.addStretch();
    m_name->setFont(QFont("Arial", 18, QFont::Bold));
    m_name->setText(displayed_author ? QString::fromStdString(displayed_author->name) : "???");
    m_nameLayout.addWidget(m_name);

    m_twitterName->setText(QString("@") + (displayed_author ? QString::fromStdString(displayed_author->screen_name) : "???"));
    m_twitterName->setFont(QFont("Arial", 12));
    m_twitterName->setStyleSheet("QLabel { color: grey; }");
    m_nameLayout.addWidget(m_twitterName);
    m_nameLayout.addStretch();

    if(!displayed_author) {
        m_dataModel->awaitUserInfo(getDisplayedAuthorId(), [this](tweeteria::User const& displayed_author) {
            // @todo TweetWidget could be dead
            m_name->setText(QString::fromStdString(displayed_author.name));
            m_twitterName->setText(QString("@") + QString::fromStdString(displayed_author.screen_name));
        });
    }

    m_menuButton->setText("+");
    m_topRowLayout.addStretch();
    m_topRowLayout.addWidget(m_menuButton);

    if(m_tweet.retweeted_status) {
        m_header->setFont(QFont("Arial", 10));
        m_header->setStyleSheet("QLabel { color: grey; }");
        m_header->setText(QString::fromStdString(m_author.name) + " retweeted");
        m_layout.addWidget(m_header);
    }

    m_layout.addStretch();
    m_text->setFont(QFont("Arial", 12));
    m_text->setText(QString::fromStdString(displayed_tweet.getDisplayText()));
    m_text->setWordWrap(true);
    m_text->setMinimumSize(512, m_text->height() * 3);
    m_text->setOpenExternalLinks(true);
    m_layout.addWidget(m_text);

    m_media->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    m_media->hide();
    m_layout.addWidget(m_media);

    m_layout.addStretch();

    m_replies->load(QByteArray(SvgIconReplySource::getIconSource(SvgIconReplySource::PressedState::Default).data()));
    m_replies->setIconSize(QSize(22, 24));
    m_actionsLayout.addWidget(m_replies);
    m_repliesLabel->setStyleSheet("QLabel { font-weight: bold; color: #AAB8C2; }");
    m_repliesLabel->setText("");
    m_actionsLayout.addWidget(m_repliesLabel);
    m_actionsLayout.addSpacing(30);

    m_retweeets->load(QByteArray(SvgIconRetweetSource::getIconSource(m_tweet.retweeted ? SvgIconRetweetSource::ActiveState::Active : SvgIconRetweetSource::ActiveState::Inactive).data()));
    m_retweeets->setIconSize(QSize(25, 24));
    m_actionsLayout.addWidget(m_retweeets);
    if(m_tweet.retweeted) {
        m_retweetsLabel->setStyleSheet("QLabel { font-weight: bold; color: #19CF86; }");
    } else {
        m_retweetsLabel->setStyleSheet("QLabel { font-weight: bold; color: #AAB8C2; }");
    }
    m_retweetsLabel->setText(QString::number(m_tweet.retweet_count));
    m_actionsLayout.addWidget(m_retweetsLabel);
    m_actionsLayout.addSpacing(30);

    m_favorites->load(QByteArray(SvgIconLikeSource::getIconSource(m_tweet.favorited ? SvgIconLikeSource::ActiveState::Active : SvgIconLikeSource::ActiveState::Inactive).data()));
    m_favorites->setIconSize(QSize(18, 24));
    m_actionsLayout.addWidget(m_favorites);
    if(m_tweet.favorited) {
        m_favoritesLabel->setStyleSheet("QLabel { font-weight: bold; color: #E81C4F; }");
    } else {
        m_favoritesLabel->setStyleSheet("QLabel { font-weight: bold; color: #AAB8C2; }");
    }
    m_favoritesLabel->setText(QString::number(m_tweet.favorite_count));
    m_actionsLayout.addWidget(m_favoritesLabel);
    m_actionsLayout.addStretch();

    m_layout.addLayout(&m_actionsLayout);

    m_date->setFont(QFont("Arial", 8));
    m_date->setStyleSheet("QLabel { color: grey; }");
    m_date->setText(QString::fromStdString(displayed_tweet.created_at));
    m_layout.addWidget(m_date);

    connect(this, &TweetWidget::imageArrived, this, &TweetWidget::onImageArrived, Qt::ConnectionType::QueuedConnection);
    connect(this, &TweetWidget::mediaArrived, this, &TweetWidget::onMediaArrived, Qt::ConnectionType::BlockingQueuedConnection);

    connect(m_menuButton, &QPushButton::clicked, this, &TweetWidget::openMenu);

    setMinimumWidth(590);

    m_layout.setContentsMargins(24, 24, 24, 24);

    setLayout(&m_layout);
}

TweetWidget::Menu::Menu(TweetWidget* parent)
    :menu(new QMenu(parent)),
     markAsRead(new QAction("&Mark as read", menu)),
     copyUrl(new QAction("&Copy URL to clipboard", menu)),
     copyJSON(new QAction("Copy &JSON to clipboard", menu)),
     openInBrowser(new QAction("&Open in external Browser...", menu))
{
    menu->addAction(markAsRead);
    connect(markAsRead, &QAction::triggered, parent, &TweetWidget::markAsRead);
    menu->addAction(copyUrl);
    connect(copyUrl, &QAction::triggered, parent, &TweetWidget::copyUrl);
    menu->addAction(copyJSON);
    connect(copyJSON, &QAction::triggered, parent, &TweetWidget::copyJSON);
    menu->addAction(openInBrowser);
    connect(openInBrowser, &QAction::triggered, parent, &TweetWidget::openInBrowser);
}

tweeteria::Tweet const& TweetWidget::getDisplayedTweet() const
{
    return (m_tweet.retweeted_status) ? (*m_tweet.retweeted_status) : m_tweet;
}

tweeteria::UserId TweetWidget::getDisplayedAuthorId() const
{
    return getDisplayedTweet().user_id;
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

void TweetWidget::openMenu()
{
    auto popup_pos = m_menuButton->pos();
    popup_pos += QPoint(0, m_menuButton->height());
    m_menu->menu->popup(QWidget::mapToGlobal(popup_pos));
}

void TweetWidget::markAsRead()
{
    GHULBUS_LOG(Trace, "markAsRead - " << m_tweet.id.id << " for author " << m_author.screen_name);
    emit markedAsRead(m_tweet.id, m_tweet.user_id);
}

void TweetWidget::copyUrl()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(QString::fromStdString(m_tweet.getUrl(m_author)));
}

void TweetWidget::copyJSON()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(QString::fromStdString(m_tweet.getPrettyJSON()));
}

void TweetWidget::openInBrowser()
{
    QDesktopServices::openUrl(QUrl(QString::fromStdString(m_tweet.getUrl(m_author)), QUrl::StrictMode));
}
