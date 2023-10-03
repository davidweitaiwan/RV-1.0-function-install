#include "MyApp.h"

namespace MyApp
{
    void AlignForWidth(float width, float alignment = 0.5f)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        float avail = ImGui::GetContentRegionAvail().x;
        float off = (avail - width) * alignment;
        if (off > 0.0f)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
    }

    float ValueMapping(float value, float leftMin, float leftMax, float rightMin, float rightMax)
    {
        float leftSpan = leftMax - leftMin;
        float rightSpan = rightMax - rightMin;
        float valueScaled = float(value - leftMin) / float(leftSpan);
        return rightMin + (valueScaled * rightSpan);
    }

    static void HelpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
        {
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    void RenderDockerUI()
    {
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags;// = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", nullptr, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
        ImGui::End();
    }

    fs::path GetHomePath()
    {
        const static uint16_t BUFF_SIZE = 256;
        char buf[BUFF_SIZE];
        FILE* fp = popen("echo $HOME", "r");
        std::string retStr = "";
        if (fp != NULL)
        {
            while (fgets(buf, BUFF_SIZE, fp) != NULL)
                retStr += buf;
            pclose(fp);
        }
        if (retStr.back() == '\n')
            retStr.pop_back();
        printf("Found home path: %s\n", retStr.c_str());
        return retStr;
    }

    std::vector<MyApp::Repo> ScanLocalPackages(const fs::path& wsDir, const std::vector<std::string>& interfaceVec)
    {
        const static uint16_t BUFF_SIZE = 256;
        char buf[BUFF_SIZE];
        std::vector<std::string> repoStrVec;
        FILE* fp = fopen((wsDir / ".modulesettings").generic_string().c_str(), "r");
        if (fp != NULL)
        {
            while (fgets(buf, BUFF_SIZE, fp) != NULL)
            {
                std::string recvStr(buf);
                recvStr = recvStr.substr(recvStr.find('#') + 1, recvStr.find('!') - 1);// #cpp_dataserver:master:eth2:dhcp:true:false!
                if (recvStr.length() > 0)
                    repoStrVec.emplace_back(recvStr);
            }
            fclose(fp);
        }

        std::vector<MyApp::Repo> ret;
        for (const auto& i : repoStrVec)// .modulesettings
        {
            auto splitStr = MyApp::split(i, ":");
            if (splitStr.size() != 6)// cpp_dataserver:master:eth2:dhcp:true:false
                continue;
            MyApp::RepoNetworkProp prop(splitStr[2], splitStr[3], splitStr[4] == "true", interfaceVec);
            {
                // Check network interface deprecated
                bool isDeprecF = true;
                for (int niIdx = 0; niIdx < interfaceVec.size(); niIdx++)
                    if (prop.interface == interfaceVec[niIdx])
                    {
                        prop.interfaceIdx = niIdx;
                        isDeprecF = false;
                        break;
                    }
                if (isDeprecF)
                {
                    prop.interface += " (deprecated)";
                    prop.interfaceVec.push_back(prop.interface);
                    prop.interfaceIdx = prop.interfaceVec.size() - 1;
                    prop.interfaceDeprecIdx = prop.interfaceIdx;
                }
            }
            ret.emplace_back(splitStr[0], splitStr[0], "", prop);
            ret.back().repoBranch = splitStr[1];
            ret.back().nonROS = splitStr[5] == "true";
        }
        return ret;
    }

    bool ReadCommonFile(const char* path, char* outStr, const size_t& outStrSize)
    {
        const static uint16_t BUFF_SIZE = 1024;
        char buf[BUFF_SIZE];

        memset(outStr, 0, outStrSize);
        FILE* fp = fopen(path, "r");
        if (fp != NULL)
        {
            while (fgets(buf, BUFF_SIZE, fp) != NULL)
                strcat(outStr, buf);
            fclose(fp);
            return true;
        }
        else
            strcat(outStr, "No Content.");
        return false;
    }

    bool ReadScriptConf(const fs::path& rvScriptConfigFile, const fs::path& rvScriptDir, std::map<std::string, std::string>& rvScripts)
    {
        std::map<std::string, std::string> rvScriptsUrl;
        // Get script url
        {
            try
            {
                nlohmann::json json;
                json.update(nlohmann::json::parse(std::ifstream(rvScriptConfigFile)));
                rvScriptsUrl["package-list"] = json["package-list"];
                rvScriptsUrl["interface-list"] = json["interface-list"];
                rvScriptsUrl["generate-startup"] = json["generate-startup"];
                rvScriptsUrl["colcon-build"] = json["colcon-build"];
                rvScriptsUrl["scan-branch"] = json["scan-branch"];
                rvScriptsUrl["scan-interface"] = json["scan-interface"];
            }
            catch (...)
            {
                printf("Read script configure file error. Put %s under the directory then restart the program.", rvScriptConfigFile.generic_string().c_str());
                return false;
            }
        }

        // Download scripts
        {
            rvScripts.clear();
            char buf[128];
            char cmdBuf[128];
            sprintf(cmdBuf, "mkdir -p %s", rvScriptDir.generic_string().c_str(), rvScriptDir.generic_string().c_str());
            system(cmdBuf);
            for (const auto& [k, url] : rvScriptsUrl)
            {
                sprintf(cmdBuf, "wget -nc -qP %s %s", rvScriptDir.generic_string().c_str(), url.c_str());
                system(cmdBuf);
                bool fileExistF = false;
                for (auto& fp : std::filesystem::directory_iterator(rvScriptDir))
                {
                    if (fp.path().filename() != k + fp.path().extension().generic_string())
                        continue;
                    rvScripts[k] = fp.path().generic_string();
                    fileExistF = true;
                    break;
                }
                if (!fileExistF)
                {
                    printf("Missing script/file: %s [%s].", k.c_str(), url.c_str());
                    return false;
                }
            }
        }
        return true;
    }

    bool SudoAuthentication(const std::string& pswd)
    {
        if (pswd.length() <= 0)
        {
            printf("Password error.\n");
            return false;
        }
        char buf[256];
        sprintf(buf, "echo '%s' | sudo -S echo 'sudo check'", pswd.c_str());
        int ret = system(buf);
        if (ret < 0)
        {
            std::cout << "Error: " << strerror(errno) << '\n';
            return false;
        }
        if (WIFEXITED(ret))
        {
            int retCode = WEXITSTATUS(ret);
            std::cout << "Program returned normally, exit code " << retCode << '\n';
            return retCode == 0;
        }
        std::cout << "Program exited abnormally\n";
        return false;
    }

    bool ScanIF(const std::string& scriptUrl, std::vector<std::string>& ifVec)
    {
        ifVec.clear();
        char buf[128];
        char cmdBuf[128];
        sprintf(cmdBuf, ". %s", scriptUrl.c_str());
        FILE* fp = popen(cmdBuf, "r");
        if (fp != NULL)
        {
            while (fgets(buf, 128, fp) != NULL)
            {
                std::string recvStr(buf);
                std::string ifNameStr = recvStr.substr(recvStr.find('^') + 1, recvStr.rfind('!') - 1);// ^eth0!
                if (ifNameStr.length() > 0 && recvStr != ifNameStr)
                    ifVec.emplace_back(ifNameStr);
            }
            pclose(fp);
        }
        ifVec.push_back("NONE");
        return true;
    }

    bool CheckROSSupport(const fs::path& packageDir)
    {
        if (!fs::exists(packageDir))
            return false;

        std::map<std::string, int> checkMap;
        
        for (const auto& entry : fs::directory_iterator(packageDir))
            checkMap[entry.path().filename().generic_string()]++;
        try
        {
            if (checkMap["CMakeLists.txt"] != 1)
                return false;
            if (checkMap["package.xml"] != 1)
                return false;
        }
        catch(const std::exception& e)
        {
            return false;
        }
        return true;
    }

    std::vector<std::string> CheckRepoBranch(const fs::path& repoDir)
    {
        std::vector<std::string> ret;
        char buf[128];
        char cmdBuf[256];
        fs::path gitDir = repoDir / ".git";
        try
        {
            if (fs::exists(gitDir))
            {
                sprintf(cmdBuf, "git --git-dir=%s branch -r | grep -v 'HEAD' | grep -Po '(?<=/)[\\w\\W]+$' | awk '{print \"^\"$0\"!\"}'", gitDir.generic_string().c_str());
                FILE* fp = popen(cmdBuf, "r");
                if (fp != NULL)
                {
                    while (fgets(buf, 128, fp) != NULL)
                    {
                        std::string recvStr(buf);
                        std::string branchStr = recvStr.substr(recvStr.find('^') + 1, recvStr.rfind('!') - 1);// ^master!
                        if (branchStr.length() > 0 && recvStr != branchStr)
                            ret.emplace_back(branchStr);
                    }
                    pclose(fp);
                }
            }
        }
        catch (...)
        {
            return {};
        }
        return ret;
    }

    bool SetPasswordBox(const std::string& btnName, AUTH& auth)
    {
        bool successF = false;
        if (ImGui::Button(btnName.c_str()))
            ImGui::OpenPopup("Set Password");
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("Set Password", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static char pswdStr[128] = "";
            ImGui::InputText("Password", pswdStr, IM_ARRAYSIZE(pswdStr), ImGuiInputTextFlags_Password);
            if (ImGui::Button("Confirm"))
            {
                successF = SudoAuthentication(pswdStr);
                if (successF)
                {
                    auth.password = pswdStr;
                    auth.isConfirmed = successF;
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        return successF;
    }

    std::vector<std::string> split(const std::string& str, const std::string& delimiter)
    {
        std::vector<std::string> splitStrings;
        int encodingStep = 0;
        for (size_t i = 0; i < str.length(); i++)
        {
            bool isDelimiter = false;
            for (auto& j : delimiter)
                if (str[i] == j)
                {
                    isDelimiter = true;
                    break;
                }
            if (!isDelimiter)// Is the spliting character
            {
                encodingStep++;
                if (i == str.length() - 1)
                    splitStrings.push_back(str.substr(str.length() - encodingStep, encodingStep));
            }
            else// Is delimiter
            {
                if (encodingStep > 0)// Have characters need to split
                    splitStrings.push_back(str.substr(i - encodingStep, encodingStep));
                encodingStep = 0;
            }
        }
        return splitStrings;
    }

    void RunInstallRemove(WorkspacePath wsPath, std::map<std::string, std::string> rvScripts, std::vector<Repo> installRepoVec, std::vector<Repo> removeRepoVec, std::vector<Repo> interVec, std::vector<Repo> installNonROSRepoVec, std::vector<Repo> removeNonROSRepoVec, std::vector<Repo> noChangeRepoVec, AUTH localAuth, bool& procF)
    {
        procF = true;
        // Run installation
        char cmdBuf[1024];
        auto contentDir = wsPath.content->path;
        auto ros2WsDir = wsPath.ros2->path;
        auto ros2SrcDir = wsPath.ros2Src->path;
        auto rvWsDir = wsPath.rv->path;
        auto rvSrcDir = wsPath.rvSrc->path;

        std::vector<Repo> outFileVec;

        printf("Create ROS2 workspace...\n");
        sprintf(cmdBuf, "mkdir -p %s", ros2SrcDir.generic_string().c_str());
        system(cmdBuf);

        printf("Update ROS2 workspace package files...\n");
        for (const auto& i : installRepoVec)
        {
            std::string tmpRepoPath = (contentDir / i.repoName).generic_string();
            std::string wsRepoPath = (ros2SrcDir / i.repoName).generic_string();

            // Remove current repo
            sprintf(cmdBuf, "rm -rf %s", wsRepoPath.c_str());
            system(cmdBuf);

            // Copy new repo to ROS2 workspace
            sprintf(cmdBuf, "cp -r %s %s", tmpRepoPath.c_str(), wsRepoPath.c_str());
            if (system(cmdBuf) != 0)
            {
                printf("Copy repo %s failed. Ignored...\n", tmpRepoPath.c_str());
                continue;
            }

            // Create startup files
            sprintf(cmdBuf, "bash %s -d %s -p %s -i %s --password %s %s", 
                rvScripts["generate-startup"].c_str(), 
                ros2WsDir.generic_string().c_str(), 
                i.repoName.c_str(), 
                i.prop.interface.c_str(), 
                localAuth.password.c_str(), 
                i.prop.internetRequired ? "--internet" : "");
            system(cmdBuf);
            outFileVec.push_back(i);
        }

        for (const auto& i : removeRepoVec)
        {
            std::string wsRepoPath = (ros2SrcDir / i.repoName).generic_string();
            // Remove startup files
            sprintf(cmdBuf, "bash %s -d %s -p %s --password %s --remove", 
                rvScripts["generate-startup"].c_str(), 
                ros2WsDir.generic_string().c_str(), 
                i.repoName.c_str(), 
                localAuth.password.c_str());
            system(cmdBuf);

            // Remove current repo
            sprintf(cmdBuf, "rm -rf %s", wsRepoPath.c_str());
            system(cmdBuf);
        }

        for (const auto& i : interVec)
        {
            std::string tmpInterfacePath = (contentDir / i.repoName).generic_string();
            std::string wsInterfacePath = (ros2SrcDir / i.repoName).generic_string();
            sprintf(cmdBuf, "rm -rf %s", wsInterfacePath.c_str());
            system(cmdBuf);
            sprintf(cmdBuf, "cp -r %s %s", tmpInterfacePath.c_str(), wsInterfacePath.c_str());
            if (system(cmdBuf) != 0)
            {
                printf("Copy interface %s failed. Force exit.\n", tmpInterfacePath.c_str());
                procF = false;
                return;
            }
        }

        // Start compile ROS2 packages
        sprintf(cmdBuf, "bash %s -d %s", 
                rvScripts["colcon-build"].c_str(), 
                ros2WsDir.generic_string().c_str());
        system(cmdBuf);


        printf("Create RV workspace...\n");
        sprintf(cmdBuf, "mkdir -p %s", rvSrcDir.generic_string().c_str());
        system(cmdBuf);

        // Non-ROS2 installation
        for (const auto& i : installNonROSRepoVec)
        {
            std::string tmpRepoPath = (contentDir / i.repoName).generic_string();
            std::string wsRepoPath = (rvSrcDir / i.repoName).generic_string();

            // Remove current repo
            sprintf(cmdBuf, "rm -rf %s", wsRepoPath.c_str());
            system(cmdBuf);

            // Copy new repo to RV workspace
            sprintf(cmdBuf, "cp -r %s %s", tmpRepoPath.c_str(), wsRepoPath.c_str());
            if (system(cmdBuf) != 0)
            {
                printf("Copy repo %s failed. Ignored...\n", tmpRepoPath.c_str());
                continue;
            }

            // Create startup files
            sprintf(cmdBuf, "bash %s -d %s -p %s -i %s --password %s %s --no-ros", 
                rvScripts["generate-startup"].c_str(), 
                rvWsDir.generic_string().c_str(), 
                i.repoName.c_str(), 
                i.prop.interface.c_str(), 
                localAuth.password.c_str(), 
                i.prop.internetRequired ? "--internet" : "");
            system(cmdBuf);
            outFileVec.push_back(i);
        }

        for (const auto& i : removeNonROSRepoVec)
        {
            std::string wsRepoPath = (rvSrcDir / i.repoName).generic_string();
            // Remove startup files
            sprintf(cmdBuf, "bash %s -d %s -p %s --password %s --remove --no-ros", 
                rvScripts["generate-startup"].c_str(), 
                rvWsDir.generic_string().c_str(), 
                i.repoName.c_str(), 
                localAuth.password.c_str());
            system(cmdBuf);

            // Remove current repo
            sprintf(cmdBuf, "rm -rf %s", wsRepoPath.c_str());
            system(cmdBuf);
        }


        // Generate .modulesettings under ros2WsDir
        FILE* fp = fopen((ros2WsDir / ".modulesettings").generic_string().c_str(), "w");
        if (fp != NULL)
        {
            for (const auto& i : outFileVec)
            {
                fprintf(fp, "#%s:%s:%s:%s:%s:%s!\n", 
                    i.repoName.c_str(), 
                    i.repoBranch.c_str(), 
                    i.prop.interface.c_str(), 
                    i.prop.ip.c_str(), 
                    i.prop.internetRequired ? "true" : "false", 
                    i.nonROS ? "true" : "false");
            }

            // For deprecated
            for (const auto& i : noChangeRepoVec)
            {
                fprintf(fp, "#%s:%s:%s:%s:%s:%s!\n", 
                    i.repoName.c_str(), 
                    i.repoBranch.c_str(), 
                    i.prop.interface.c_str(), 
                    i.prop.ip.c_str(), 
                    i.prop.internetRequired ? "true" : "false", 
                    i.nonROS ? "true" : "false");
            }
            fclose(fp);
        }
        procF = false;
    }

    char** StrVecToCStrArr(const std::vector<std::string>& strVec)
    {
        char **ret = new char*[strVec.size()];
        for (int i = 0; i < strVec.size(); i++)
        {
            ret[i] = new char[strVec[i].length() + 1];
            memset(ret[i], 0, strVec[i].length() + 1);
            memcpy(ret[i], strVec[i].data(), strVec[i].length());
        }
        return ret;
    }
}
