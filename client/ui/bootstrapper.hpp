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
#ifndef TWEETERIA_CLIENT_INCLUDE_GUARD_UI_BOOTSTRAPPER_HPP
#define TWEETERIA_CLIENT_INCLUDE_GUARD_UI_BOOTSTRAPPER_HPP

#include <QObject>

#include <tweeteria/proxy_config.hpp>

#include <memory>

class Bootstrapper : public QObject
{
    Q_OBJECT
private:
    struct Pimpl;
    std::unique_ptr<Pimpl> m_pimpl;

    tweeteria::ProxyConfig m_proxyConfig;
    int m_connectivityTestGenerationCount;
public:
    Bootstrapper(QObject* parent);

    ~Bootstrapper();

    Bootstrapper(Bootstrapper const&) = delete;
    Bootstrapper& operator=(Bootstrapper const&) = delete;

    void checkConnectivity();

signals:
    void connectivityCheckStarted(int generation_count);
    void connectivityCheckSucceeded(int generation_count);
    void connectivityCheckFailed(int generation_count, QString const& reason);

public slots:
    void onProxyConfigurationChange(tweeteria::ProxyConfig new_proxy_config);
};

#endif
