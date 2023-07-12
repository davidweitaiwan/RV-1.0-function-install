#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <iostream>
#include <fstream>
#include <filesystem>

#include <map>
#include <deque>
#include <vector>
#include <string>
#include <cstring>

#include <thread>
#include <chrono>

#include "imgui.h"

namespace MyApp
{
    typedef std::pair<bool, bool> ModSign;
    class RepoProp;
    struct Repo;
    struct AUTH;
    static void HelpMarker(const char* desc);
    float ValueMapping(float value, float leftMin, float leftMax, float rightMin, float rightMax);
    void RenderDockerUI();
    std::filesystem::path GetHomePath();
    std::vector<MyApp::Repo> ScanLocalPackages(const std::filesystem::path& ros2WsDir);
    bool ReadCommonFile(const char* path, char* outStr, const size_t& outStrSize);
    bool SudoAuthentication(const std::string& pswd);
    bool SetPasswordBox(const std::string& btnName, AUTH& pswd);
    std::vector<std::string> split(const std::string& str, const std::string& delimiter);

    class RepoProp
    {
    public:
        std::string interface;// Default: eth0
        std::string ip;// Default: dhcp
        bool internetRequired;// Default: false; true if module needs internet connection

        RepoProp() :
            interface("eth0"), 
            ip("dhcp"), 
            internetRequired(false) {}
        
        RepoProp(const std::string& interface, const std::string& ip, bool internetRequired) :
            interface(interface), 
            ip(ip), 
            internetRequired(internetRequired) {}
        
        RepoProp(const RepoProp& prop)
        {
            this->interface = prop.interface;
            this->ip = prop.ip;
            this->internetRequired = prop.internetRequired;
        }
        
        bool operator==(const RepoProp& item)
        {
            if (this->interface != item.interface)
                return false;
            else if (this->ip != item.ip)
                return false;
            else if (this->internetRequired != item.internetRequired)
                return false;
            return true;
        }
    };

    struct Repo
    {
        std::string repoDescribe;
        std::string repoName;
        std::string repoUrl;
        RepoProp prop;

        Repo(const std::string& des, const std::string& name, const std::string& url) : 
            repoDescribe(des), 
            repoName(name), 
            repoUrl(url) {}
        
        Repo(const std::string& des, const std::string& name, const std::string& url, const RepoProp& prop) : 
            repoDescribe(des), 
            repoName(name), 
            repoUrl(url), 
            prop(prop) {}
    };

    struct AUTH
    {
        std::string account = "";
        std::string password = "";
        bool isConfirmed = false;
    };

    struct ColoredString
    {
        std::string str;
        ImVec4 col;

        ColoredString()
        {
            str = "";
            col = {1, 1, 1, 1};
        }

        void err(const std::string& _str)
        {
            str = _str;
            col = {1, 0, 0, 1};
        }

        void succ(const std::string& _str)
        {
            str = _str;
            col = {0, 1, 0, 1};
        }

        void warn(const std::string& _str)
        {
            str = _str;
            col = {1, 1, 0, 1};
        }

        void info(const std::string& _str)
        {
            str = _str;
            col = {1, 1, 1, 1};
        }
    };

    struct MirrorPath
    {
        std::filesystem::path path;
        char c_str[256];

        MirrorPath(const std::filesystem::path& _path)
        {
            path = _path;
            strcpy(c_str, path.generic_string().c_str());
        }

        void update(const std::string& _path)
        {
            path = _path;
            strcpy(c_str, path.generic_string().c_str());
        }

        void updatePath()
        {
            path = c_str;
        }

        void updateStr()
        {
            strcpy(c_str, path.generic_string().c_str());
        }
    };

    template<typename T>
    class Scope
    {
    private:
        std::string name_;
        std::map<double, T, std::less<double> > data_;
        std::deque<T> showQue_;
        uint32_t queSz_;

        int direction_;

    public:
        Scope(std::string name, uint32_t queSize = 200) : name_(name), queSz_(queSize), direction_(1)
        {
            this->showQue_ = std::deque<T>(queSize);
        }

        void add(double ts, T data)
        {
            this->data_[ts] = data;
        }

        std::deque<T> updateNextQue(double ts, int direction = 1)
        {
            if (direction)
            {
                this->showQue_.pop_front();
                this->showQue_.push_back(this->data_[ts]);
            }
            return this->showQue_;
        }
    };
}
