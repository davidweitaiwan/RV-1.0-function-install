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

    std::vector<MyApp::Repo> ScanLocalPackages(const fs::path& ros2WsDir)
    {
        const static uint16_t BUFF_SIZE = 256;
        char buf[BUFF_SIZE];
        std::vector<std::string> repoStrVec;
        FILE* fp = fopen((ros2WsDir / ".modulesettings").generic_string().c_str(), "r");
        if (fp != NULL)
        {
            while (fgets(buf, BUFF_SIZE, fp) != NULL)
            {
                std::string recvStr(buf);
                recvStr = recvStr.substr(recvStr.find('#') + 1, recvStr.find('!') - 1);// #cpp_dataserver:eth2:dhcp:true!
                if (recvStr.length() > 0)
                    repoStrVec.emplace_back(recvStr);
            }
            fclose(fp);
        }

        std::vector<MyApp::Repo> ret;
        for (const auto& i : repoStrVec)// .modulesettings
        {
            auto splitStr = MyApp::split(i, ":");
            if (splitStr.size() != 4)// cpp_dataserver:eth2:dhcp:true
                continue;
            MyApp::RepoProp prop(splitStr[1], splitStr[2], (splitStr[3] == "true" ? true : false));
            ret.emplace_back(splitStr[0], splitStr[0], "", prop);
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

    bool SetPasswordBox(const std::string& btnName, AUTH& pswd)
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
                    pswd.password = pswdStr;
                    pswd.isConfirmed = successF;
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
}
