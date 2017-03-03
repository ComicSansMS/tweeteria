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

#include <image_provider.hpp>

#include <web_resource_provider.hpp>

ImageProvider::ImageProvider(WebResourceProvider& provider)
    :m_provider(&provider)
{
}

void ImageProvider::retrieve(std::string const& url, std::function<void(QPixmap)> retrieval_cb)
{
    m_provider->retrieve(url, [retrieval_cb](std::vector<unsigned char> const& data) {
        QPixmap qpm;
        qpm.loadFromData(data.data(), static_cast<uint>(data.size()));
        retrieval_cb(qpm);
    }, CachingPolicy::NoCaching);
}
