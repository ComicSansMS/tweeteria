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
#include <ui/main_window.hpp>

#include <ui/bootstrapper.hpp>
#include <ui/central_widget.hpp>
#include <ui/opening_dialog.hpp>
#include <ui/user_widget.hpp>
#include <ui/tweet_widget.hpp>

#include <metatype_declarations.hpp>
#include <image_provider.hpp>
#include <web_resource_provider.hpp>

#include <tweeteria/tweeteria.hpp>
#include <tweeteria/image_util.hpp>
#include <tweeteria/string_util.hpp>

#include <gbBase/Finally.hpp>
#include <gbBase/Log.hpp>
#include <gbBase/LogHandlers.hpp>

#include <db/client_database.hpp>

#include <cpprest/http_client.h>

#include <QApplication>
#include <QMessageBox>

#include <fstream>
#include <unordered_map>

void registerMetatypes()
{
    qRegisterMetaType<tweeteria::User>();
    qRegisterMetaType<tweeteria::Tweet>();
}

int main(int argc, char* argv[])
{
    QApplication theApp(argc, argv);

    Ghulbus::Log::initializeLogging();
    auto log_guard = Ghulbus::finally(Ghulbus::Log::shutdownLogging);
    Ghulbus::Log::Handlers::LogToFile file_handler("tweeteria.log");
#if defined WIN32 && defined _DEBUG
    Ghulbus::Log::Handlers::LogMultiSink middle_handler(file_handler, Ghulbus::Log::Handlers::logToWindowsDebugger);
    Ghulbus::Log::setLogLevel(Ghulbus::LogLevel::Trace);
#else
    auto& middle_handler = file_handler;
    Ghulbus::Log::setLogLevel(Ghulbus::LogLevel::Warning);
#endif
    Ghulbus::Log::Handlers::LogAsync top_handler(middle_handler);
    top_handler.start();
    auto top_handler_guard = Ghulbus::finally([&top_handler]() { top_handler.stop(); });
    Ghulbus::Log::setLogHandler(top_handler);

    GHULBUS_LOG(Info, "Tweeteria client up and running.");

    registerMetatypes();

    Bootstrapper bootstrap(nullptr);

    OpeningDialog od;
    QObject::connect(&bootstrap, &Bootstrapper::connectivityCheckStarted,
                     &od, &OpeningDialog::onStartConnectivityTest);
    QObject::connect(&bootstrap, &Bootstrapper::connectivityCheckSucceeded,
                     &od, &OpeningDialog::onConnectivityTestSuccessful, Qt::QueuedConnection);
    QObject::connect(&bootstrap, &Bootstrapper::connectivityCheckFailed,
                     &od, &OpeningDialog::onConnectivityTestFailed, Qt::QueuedConnection);
    QObject::connect(&od, &OpeningDialog::proxyConfigurationChanged, &bootstrap, &Bootstrapper::onProxyConfigurationChange);
    QObject::connect(&bootstrap, &Bootstrapper::oauthAuthorizationUrlReady,
                     &od, &OpeningDialog::onOAuthUrlArrived, Qt::QueuedConnection);

    QObject::connect(&bootstrap, &Bootstrapper::credentialsVerified,
                     &od, &OpeningDialog::onCredentialsVerified, Qt::QueuedConnection);
    QObject::connect(&od, &OpeningDialog::oauthPinEntered, &bootstrap, &Bootstrapper::onOAuthPinEntered);
    od.show();

    bootstrap.checkConnectivity();

    std::unique_ptr<MainWindow> main_window;
    QObject::connect(&bootstrap, &Bootstrapper::credentialsVerified,
                     &bootstrap, [&bootstrap, &od, &main_window]() 
        {
            main_window = std::make_unique<MainWindow>(bootstrap.getTweeteria(), *bootstrap.getVerifiedUser());
            main_window->populateUsers();
            QObject::connect(&od, &OpeningDialog::go, main_window.get(), [&main_window, &od]() { main_window->show(); od.close(); });
        }, Qt::QueuedConnection);

    return theApp.exec();
}

