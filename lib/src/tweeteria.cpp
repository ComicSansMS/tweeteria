
#include <tweeteria/tweeteria.hpp>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

#include <iostream>
#include <fstream>
#include <string>

namespace tweeteria
{
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
                users.push_back(User::fromJSon(user_arr[i]));
            }
        }
    }
    for(auto const& u : users) {
        std::cout << "\n\n-------------------------------------------------\n";
        std::cout << u << std::endl;
    }
    return users;
}

std::vector<Tweet> json_test_tweets()
{
    std::ifstream fin("timeline.txt");
    std::string str;
    std::getline(fin, str);
    rapidjson::Document d;
    d.Parse(str.data(), str.length());
    std::vector<Tweet> tweets;
    for(rapidjson::SizeType i=0; i<d.Size(); ++i) {
        tweets.push_back(Tweet::fromJSon(d[i]));
    }
    return tweets;
}
}
