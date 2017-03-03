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
#ifndef TWEETERIA_CLIENT_INCLUDE_GUARD_IMAGE_PROVIDER_HPP
#define TWEETERIA_CLIENT_INCLUDE_GUARD_IMAGE_PROVIDER_HPP

#include <QPixmap>

#include <future>
#include <unordered_map>

class WebResourceProvider;

class ImageProvider {
private:
    WebResourceProvider* m_provider;
    std::unordered_map<std::string, QPixmap> m_cache;
public:
    ImageProvider(WebResourceProvider& provider);

    ImageProvider(ImageProvider const&) = delete;
    ImageProvider& operator=(ImageProvider const&) = delete;

    void retrieve(std::string const& url, std::function<void(QPixmap)> retrieval_cb);
};

#endif
