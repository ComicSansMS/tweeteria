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
#include <cpprest/http_client.h>
#include <cpprest/oauth1.h>

#include <array>
#include <cctype>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#include <boost/optional.hpp>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#undef NOMINMAX
#undef WIN32_LEAN_AND_MEAN

void putTextToClipboard(utility::string_t const& str, HWND hWnd = nullptr)
{
    HGLOBAL hMemory = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(wchar_t) * (str.length() + 1));
    if(!hMemory) {
        throw std::bad_alloc();
    }
    wchar_t* memptr = static_cast<wchar_t*>(GlobalLock(hMemory));
    std::memcpy(memptr, str.data(), sizeof(wchar_t) * str.length());
    GlobalUnlock(hMemory);
    BOOL res = OpenClipboard(hWnd);
    if(!res) {
        throw std::runtime_error("Unable to open clipboard.");
    }
    HANDLE hClipboard = SetClipboardData(CF_UNICODETEXT, hMemory);
    if(!hClipboard) {
        auto const err = GetLastError();
        GlobalFree(hMemory);
        throw std::runtime_error("Unable to set clipboard.");
    }
    res = CloseClipboard();
    if(!res) {
        throw std::runtime_error("Unable to close clipboard.");
    }
}

boost::optional<web::http::oauth1::experimental::oauth1_token> tokenFromFile()
{
    std::ifstream fin("token.dat", std::ios_base::binary);
    if(!fin) {
        return boost::none;
    }
    std::uint64_t access_token_size;
    fin.read(reinterpret_cast<char*>(&access_token_size), sizeof(access_token_size));
    std::vector<utility::string_t::value_type> read_buffer;
    read_buffer.resize((access_token_size / sizeof(utility::string_t::value_type)), L'0');
    fin.read(reinterpret_cast<char*>(read_buffer.data()), access_token_size);
    utility::string_t access_token(begin(read_buffer), end(read_buffer));

    std::uint64_t token_secret_size;
    fin.read(reinterpret_cast<char*>(&token_secret_size), sizeof(token_secret_size));
    read_buffer.clear();
    read_buffer.resize((token_secret_size / sizeof(utility::string_t::value_type)), L'0');
    fin.read(reinterpret_cast<char*>(read_buffer.data()), token_secret_size);
    utility::string_t token_secret(begin(read_buffer), end(read_buffer));

    if(!fin) {
        return boost::none;
    }
    return web::http::oauth1::experimental::oauth1_token(access_token, token_secret);
}

void tokenToFile(web::http::oauth1::experimental::oauth1_token const& token)
{
    if(!token.is_valid_access_token()) { throw std::logic_error("Cannot serialize invalid token."); }
    std::ofstream fout("token.dat", std::ios_base::binary);
    auto const access_token = token.access_token();
    std::uint64_t access_token_size = access_token.length() * sizeof(utility::string_t::value_type);
    fout.write(reinterpret_cast<char const*>(&access_token_size), sizeof(access_token_size));
    fout.write(reinterpret_cast<char const*>(access_token.data()), access_token_size);

    auto const token_secret = token.secret();
    std::uint64_t token_secret_size = token_secret.length() * sizeof(utility::string_t::value_type);
    fout.write(reinterpret_cast<char const*>(&token_secret_size), sizeof(token_secret_size));
    fout.write(reinterpret_cast<char const*>(token_secret.data()), token_secret_size);
    if(!fout) {
        throw std::runtime_error("Error serializing token data.");
    }
}

void obtainToken(web::http::oauth1::experimental::oauth1_config& cfg)
{
    cfg.build_authorization_uri().then([&cfg](utility::string_t const& auth_url)
    {
        putTextToClipboard(auth_url);
        std::wstringstream wsstr;
        wsstr << L"Visit the following URL to authenticate Tweeteria:\n\n" << auth_url
              << L"\n\nPress Okay once you have authenticated Tweeteria on the website."
              << L"\nThe URL has also been copied to your clipboard.";
        MessageBoxW(nullptr, wsstr.str().c_str(), L"Tweeteria awaiting authentication.", MB_OK | MB_ICONINFORMATION);
        std::array<unsigned char, 7> pin;
        for(;;) {
            std::cout << "Enter PIN: ";
            std::string pin_str;
            std::cin >> pin_str;
            int pin_idx = 0;
            for(auto const& c : pin_str) {
                if(std::isdigit(c)) {
                    pin[pin_idx] = c - '0';
                    pin_idx++;
                    if(pin_idx == 7) { break; }
                }
            }
            if(pin_idx == 7) {
                std::cout << "Your PIN is ";
                for(auto const& p : pin) { std::cout << static_cast<unsigned int>(p); }
                std::cout << std::endl;
                break;
            }
            std::cout << "Not a valid PIN. A valid PIN consists of 7 numerical digits.\n" << std::endl;
        }
        web::uri_builder uri_builder;
        utility::string_t pin_string;
        std::transform(pin.begin(), pin.end(), std::back_inserter(pin_string), [](unsigned char c) -> wchar_t { return c + L'0'; });

        return cfg.token_from_verifier(pin_string);
    }).wait();
}

int main()
{
    utility::string_t const consumer_key = U("/* redacted */");
    utility::string_t const consumer_secret = U("/* redacted */");
    utility::string_t const temp_endpoint = U("https://api.twitter.com/oauth/request_token");
    utility::string_t const auth_endpoint = U("https://api.twitter.com/oauth/authenticate");
    utility::string_t const token_endpoint = U("https://api.twitter.com/oauth/access_token");
    utility::string_t const callback_uri = U("oob");
    auto cfg_ptr = std::make_shared<web::http::oauth1::experimental::oauth1_config>(consumer_key, consumer_secret,
        temp_endpoint, auth_endpoint, token_endpoint,callback_uri,
        web::http::oauth1::experimental::oauth1_methods::hmac_sha1);
    web::http::oauth1::experimental::oauth1_config& cfg = *cfg_ptr;
    auto const opt_token = tokenFromFile();
    if(opt_token) {
        cfg.set_token(*opt_token);
    } else {
        try {
            obtainToken(cfg);
        } catch(std::exception& e) {
            std::cout << "ERROR!\n" << e.what() << std::endl;
            return 1;
        }
        tokenToFile(cfg.token());
    }
    auto const token = cfg.token();
    std::wcout << "Token: Access " << token.access_token() << " Secret " << token.secret() << " Valid " << ((int)token.is_valid_access_token()) << std::endl;

    auto oauth1_handler_ptr = std::make_shared<web::http::oauth1::details::oauth1_handler>(cfg_ptr);
    web::http::client::http_client client(U("https://api.twitter.com/1.1/"));
    client.add_handler(oauth1_handler_ptr);

    {
        web::http::uri_builder builder(U("/account/verify_credentials.json"));
        client.request(web::http::methods::GET, builder.to_string()).then([](web::http::http_response response)
        {
            std::wcout << "Received response:\n" << response.to_string() << std::endl;
        }).wait();
    }

    if(0)
    {
        web::http::uri_builder builder(U("/friends/list.json"));
        builder.append_query(U("cursor"), U("1454852922780141802"));
        client.request(web::http::methods::GET, builder.to_string()).then([](web::http::http_response response)
        {
            return response.extract_string();
        }).then([](utility::string_t const& body) {
            std::wofstream fout("friends.txt");
            fout << body;
        }).wait();
    }

    if(0)
    {
        web::http::uri_builder builder(U("/statuses/user_timeline.json"));
        builder.append_query(U("user_id"), U("65135642"));
        builder.append_query(U("max_id"), U("823746483316932608"));
        client.request(web::http::methods::GET, builder.to_string()).then([](web::http::http_response response)
        {
            return response.extract_string();
        }).then([](utility::string_t const& body) {
            std::wofstream fout("timeline.txt");
            fout << body;
        }).wait();
    }

    {
        web::http::uri_builder builder(U("/statuses/lookup.json"));
        builder.append_query(U("id"), U("828546674175905792"));
        client.request(web::http::methods::GET, builder.to_string()).then([](web::http::http_response response)
        {
            return response.extract_string();
        }).then([](utility::string_t const& body) {
            std::wofstream fout("tweet.txt");
            fout << body;
        }).wait();
    }

    return 0;
}
