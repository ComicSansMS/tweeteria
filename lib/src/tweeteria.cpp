
#include <tweeteria/tweeteria.hpp>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

#include <cpprest/json.h>

#include <iostream>
#include <fstream>
#include <string>

namespace tweeteria
{

User fromJSon(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw std::runtime_error(""); }
    User ret;
    ret.id = val["id"].GetUint64();
    ret.name = val["name"].GetString();
    ret.screen_name = val["screen_name"].GetString();
    ret.description = val["description"].GetString();
    ret.followers_count = val["followers_count"].GetUint();
    ret.friends_count = val["friends_count"].GetUint();
    ret.favourites_count = val["favourites_count"].GetUint();
    ret.profile_image_url_https = val["profile_image_url_https"].GetString();
    return ret;
}

std::ostream& operator<<(std::ostream& os, User const& u)
{
    os << "@" << u.screen_name << " (" <<  u.name << ")\n" << u.description
       << "\nFollowers: " << u.followers_count << " Following: " << u.friends_count << " Favourites: " << u.favourites_count;
    return os;
}

std::vector<User> json_test()
{
    std::ifstream fin("friends.txt");
    std::string str;
    std::getline(fin, str);
    rapidjson::Document d;
    d.Parse(str.data(), str.length());
    bool is_obj = d.IsObject();
    std::vector<User> users;
    for(auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
        std::cout << (it->name.GetString()) << std::endl;
        if(it->name == "users") {
            auto& user_arr = it->value.GetArray();
            for(rapidjson::SizeType i=0; i<user_arr.Size(); ++i) {
                users.push_back(fromJSon(user_arr[i]));
            }
        }
    }
    for(auto const& u : users) {
        std::cout << "\n\n-------------------------------------------------\n";
        std::cout << u << std::endl;
    }
    return users;
}


}
