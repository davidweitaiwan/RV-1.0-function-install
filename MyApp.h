#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <iostream>
#include <fstream>

// Ref: https://stackoverflow.com/a/55475023
#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __cplusplus >= 201703L && __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp>
     namespace fs = boost::filesystem;
#  endif
#endif

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
class RepoNetworkProp;
struct Repo;
struct AUTH;
static void HelpMarker(const char* desc);
float ValueMapping(float value, float leftMin, float leftMax, float rightMin, float rightMax);
void RenderDockerUI();
fs::path GetHomePath();
std::vector<MyApp::Repo> ScanLocalPackages(const fs::path& ros2WsDir, const std::vector<std::string>& interfaceVec);
void UpdateRepoBranch(Repo& repo, AUTH& auth, const std::string& scriptUrl);
bool ReadCommonFile(const char* path, char* outStr, const size_t& outStrSize);
bool SudoAuthentication(const std::string& pswd);
bool SetPasswordBox(const std::string& btnName, AUTH& auth);
std::vector<std::string> split(const std::string& str, const std::string& delimiter);

char** StrVecToCStrArr(const std::vector<std::string>& str);

class RepoNetworkProp
{
public:
    std::string interface;// Default: NONE
    std::vector<std::string> interfaceVec;
    int interfaceIdx;// Default: 0
    int interfaceDeprecIdx;// Default: -1
    std::string ip;// Default: dhcp
    bool internetRequired;// Default: false; true if module needs internet connection

    RepoNetworkProp(const std::vector<std::string>& interfaceVec) :
        interface("NONE"), 
        interfaceVec(interfaceVec), 
        interfaceIdx(0), 
        interfaceDeprecIdx(-1), 
        ip("dhcp"), 
        internetRequired(false) {}
    
    RepoNetworkProp(const std::string& interface, const std::string& ip, bool internetRequired, const std::vector<std::string>& interfaceVec) :
        interface(interface), 
        interfaceVec(interfaceVec), 
        interfaceIdx(0), 
        interfaceDeprecIdx(-1), 
        ip(ip), 
        internetRequired(internetRequired) {}
    
    RepoNetworkProp(const RepoNetworkProp& prop)
    {
        this->interface = prop.interface;
        this->interfaceVec = prop.interfaceVec;
        this->interfaceIdx = prop.interfaceIdx;
        this->interfaceDeprecIdx = prop.interfaceDeprecIdx;
        this->ip = prop.ip;
        this->internetRequired = prop.internetRequired;
    }
    
    bool operator==(const RepoNetworkProp& item)
    {
        return this->interface == item.interface && 
                this->ip == item.ip && 
                this->internetRequired == item.internetRequired;
    }
};

struct Repo
{
    std::string repoDescribe;
    std::string repoName;
    std::string repoUrl;
    std::string repoBranch;
    std::vector<std::string> repoBranchVec;
    int repoBranchIdx;// < 0 if not set by combo
    int repoBranchDeprecIdx;
    RepoNetworkProp prop;

    Repo(const std::string& des, const std::string& name, const std::string& url, const std::vector<std::string>& interfaceVec) : 
        repoDescribe(des), 
        repoName(name), 
        repoUrl(url), 
        repoBranch("master"), 
        repoBranchIdx(0), 
        repoBranchDeprecIdx(-1), 
        prop(interfaceVec) {}
    
    Repo(const std::string& des, const std::string& name, const std::string& url, const RepoNetworkProp& prop) : 
        repoDescribe(des), 
        repoName(name), 
        repoUrl(url), 
        repoBranch("master"), 
        repoBranchIdx(0), 
        repoBranchDeprecIdx(-1), 
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
    fs::path path;
    char c_str[256];

    MirrorPath(const fs::path& _path)
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
