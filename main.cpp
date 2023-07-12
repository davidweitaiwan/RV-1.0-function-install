// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs
# define _CRT_SECURE_NO_WARNINGS
#include "MyApp.h"
#include "imgui.h"
//#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

using namespace std::chrono_literals;

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

#include <math.h>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int main(int argc, char** argv)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Module Installer", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);



    // Variable Definitions
    const static size_t PATH_BUF_SIZE = 256;
    // FTP urls
    std::string ftpFuncList = "ftp://61.220.23.239/rv-functions/package-list.txt";
    std::string ftpInterList = "ftp://61.220.23.239/rv-functions/interface-list.txt";
    std::string ftpGenStartupScript = "ftp://61.220.23.239/rv-functions/generate-startup.sh";
    std::string ftpCompileScript = "ftp://61.220.23.239/rv-functions/colcon-build.sh";

    // Work directory
    MyApp::MirrorPath tmpDir("./.function_install_tmp");
    MyApp::MirrorPath ros2WsDir(MyApp::GetHomePath() / "ros2_ws");
    MyApp::MirrorPath ros2SrcDir(ros2WsDir.path / "src");
    
    // Authentication
    MyApp::AUTH localAuth;
    MyApp::AUTH ftpAuth;
    MyApp::MirrorPath ftpKeyPath("ftp.key");

    // Repository vectors
    std::vector<std::pair<MyApp::Repo, MyApp::ModSign> > repoVec;// {repo, installF}
    std::vector<std::pair<MyApp::Repo, MyApp::ModSign> > deprecRepoVec;// {repo, installF}
    std::vector<MyApp::Repo> installRepoVec;
    std::vector<MyApp::Repo> removeRepoVec;

    // Color definitions
    ImVec4 repoColorNoChange(1, 1, 1, 1);
    ImVec4 repoColorInstall(0, 1, 0, 1);
    ImVec4 repoColorRemove(1, 0, 0, 1);

    // Interface vectors
    std::vector<MyApp::Repo> interVec;

    // Launch file path
    std::filesystem::path launchFilePath = "common.yaml";
    const static size_t LAUNCH_CONTENT_SIZE = 1024 * 128;
    static char launchFile[LAUNCH_CONTENT_SIZE];
    bool launchFileF = false;

    MyApp::ColoredString launchCmdStateStr;

    // Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = NULL;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImFont* globFont = ImGui::GetFont();
        globFont->Scale = 2;

        ImFont contentFont = *ImGui::GetFont();
        contentFont.Scale *= 0.8;

        // My code goes here
        MyApp::RenderDockerUI();

        {
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("Authentication"))
                {
                    ImGui::SeparatorText("Local Authentication");
                    if (localAuth.isConfirmed)
                        ImGui::TextColored({0, 1, 0, 1}, "Password Confirmed!");
                    else
                        ImGui::TextColored({1, 0, 0, 1}, "Password Not Confirmed.");
                    ImGui::SameLine();
                    if (MyApp::SetPasswordBox("Set", localAuth))
                        launchCmdStateStr.succ("Authentication Passed!");
                    // TODO: ftp key auth (future work)
                    ImGui::SeparatorText("FTP Authentication");
                    ImGui::InputText("FTP Key", ftpKeyPath.c_str, sizeof(ftpKeyPath.c_str));
                    ImGui::SameLine();
                    if (ImGui::Button("Set"))
                    {
                        ftpKeyPath.updatePath();
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("File"))
                {
                    ImGui::InputText("Temporary Directory", tmpDir.c_str, sizeof(tmpDir.c_str));
                    ImGui::InputText("ROS2 Workspace", ros2WsDir.c_str, sizeof(ros2WsDir.c_str));
                    if (ImGui::Button("Set"))
                    {
                        if (strcmp(tmpDir.c_str, tmpDir.path.generic_string().c_str()) != 0 || strcmp(ros2WsDir.c_str, ros2WsDir.path.generic_string().c_str()) != 0 )
                            ImGui::OpenPopup("Path Changes");
                    }
                    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                    if (ImGui::BeginPopupModal("Path Changes", NULL, ImGuiWindowFlags_AlwaysAutoResize))
                    {
                        ImGui::Separator();
                        if (ImGui::BeginTable("pathChanges", 3))
                        {
                            ImVec4 modColor;
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            if (strcmp(tmpDir.c_str, tmpDir.path.generic_string().c_str()) == 0)
                                modColor = repoColorNoChange;
                            else
                                modColor = repoColorInstall;
                            ImGui::TextColored(modColor, tmpDir.path.generic_string().c_str());
                            ImGui::TableNextColumn();
                            ImGui::TextColored(modColor, " >> ");
                            ImGui::TableNextColumn();
                            ImGui::TextColored(modColor, tmpDir.c_str);

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            if (strcmp(ros2WsDir.c_str, ros2WsDir.path.generic_string().c_str()) == 0)
                                modColor = repoColorNoChange;
                            else
                                modColor = repoColorInstall;
                            ImGui::TextColored(modColor, ros2WsDir.path.generic_string().c_str());
                            ImGui::TableNextColumn();
                            ImGui::TextColored(modColor, " >> ");
                            ImGui::TableNextColumn();
                            ImGui::TextColored(modColor, ros2WsDir.c_str);
                            ImGui::EndTable();
                        }

                        ImGui::Separator();
                        static bool fileRemoveF = false;
                        ImGui::Checkbox("Remove Current Files", &fileRemoveF);
                        if (ImGui::Button("OK", ImVec2(150, 0)))
                        {
                            if (fileRemoveF)
                            {
                                auto vec = MyApp::ScanLocalPackages(ros2WsDir.path);
                                if (localAuth.isConfirmed)
                                {
                                    char buf[PATH_BUF_SIZE];
                                    sprintf(buf, "sudo rm -rf %s", tmpDir.path.generic_string().c_str());
                                    system(buf);
                                    for (const auto& i : vec)
                                    {
                                        // Remove startup files
                                        sprintf(buf, "curl -fsSL %s | bash -s -- -d %s -t %s -p %s --password %s --remove", 
                                            ftpGenStartupScript.c_str(), 
                                            ros2WsDir.path.generic_string().c_str(), 
                                            tmpDir.path.generic_string().c_str(), 
                                            i.repoName.c_str(), 
                                            localAuth.password.c_str());
                                        system(buf);
                                    }
                                    sprintf(buf, "sudo rm -rf %s", ros2WsDir.path.generic_string().c_str());
                                    system(buf);
                                }
                                else
                                    printf("Authentication Failed! File not removed.\n");
                            }
                            tmpDir.updatePath();
                            ros2WsDir.updatePath();
                            ros2SrcDir.update(ros2WsDir.path / "src");
                            launchCmdStateStr.info("Path Changed!");
                            fileRemoveF = false;
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();;
                        if (ImGui::Button("Cancel", ImVec2(150, 0)))
                        {
                            tmpDir.updateStr();
                            ros2WsDir.updateStr();
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Reset"))
                    {
                        tmpDir.updateStr();
                        ros2WsDir.updateStr();
                    }
                    // Remove button
                    ImGui::Separator();
                    if (ImGui::Button("Remove Installed Files"))
                    {
                        if (localAuth.isConfirmed)
                        {
                            auto vec = MyApp::ScanLocalPackages(ros2WsDir.path);
                            char buf[PATH_BUF_SIZE];
                            for (const auto& i : vec)
                            {
                                // Remove startup files
                                sprintf(buf, "curl -fsSL %s | bash -s -- -d %s -t %s -p %s --password %s --remove", 
                                    ftpGenStartupScript.c_str(), 
                                    ros2WsDir.path.generic_string().c_str(), 
                                    tmpDir.path.generic_string().c_str(), 
                                    i.repoName.c_str(), 
                                    localAuth.password.c_str());
                                system(buf);
                            }
                            sprintf(buf, "sudo rm -rf %s", ros2WsDir.path.generic_string().c_str());
                            system(buf);
                        }
                        else
                            launchCmdStateStr.err("Authentication Failed!");
                    }
                    if (ImGui::Button("Remove Temporary Files"))
                    {
                        if (localAuth.isConfirmed)
                        {
                            char buf[PATH_BUF_SIZE];
                            sprintf(buf, "sudo rm -rf %s", tmpDir.path.generic_string().c_str());
                            system(buf);
                        }
                        else
                            launchCmdStateStr.err("Authentication Failed!");
                    }

                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
        }

        // Repo Window
        {
            ImGui::Begin("Repo");

            if (ImGui::Button("Scan"))// TODO: functionalize. Add non-ROS2 function installation.
            {
                interVec.clear();
                repoVec.clear();

                char buf[PATH_BUF_SIZE];
                char cmdBuf[PATH_BUF_SIZE];
                
                std::vector<std::string> repoStrVec;

                // Online interface list check
                sprintf(cmdBuf, "curl -s %s", ftpInterList.c_str());
                FILE* fp = popen(cmdBuf, "r");
                if (fp != NULL)
                {
                    while (fgets(buf, PATH_BUF_SIZE, fp) != NULL)
                    {
                        std::string recvStr(buf);
                        recvStr = recvStr.substr(recvStr.find('^') + 1, recvStr.find('!') - 1);// @Vehicle Interfaces@vehicle_interfaces@https://github.com/.../RV-1.0-vehicle_interfaces.git!
                        if (recvStr.length() > 0)
                            repoStrVec.emplace_back(recvStr);
                    }
                    pclose(fp);
                }
                
                for (const auto& i : repoStrVec)// interface-list
                {
                    auto splitStr = MyApp::split(i, "@");
                    if (splitStr.size() != 3)
                        continue;
                    interVec.emplace_back(splitStr[0], splitStr[1], splitStr[2]);
                }

                // Online module list check
                repoStrVec.clear();
                sprintf(cmdBuf, "curl -s %s", ftpFuncList.c_str());
                fp = popen(cmdBuf, "r");
                if (fp != NULL)
                {
                    while (fgets(buf, PATH_BUF_SIZE, fp) != NULL)
                    {
                        std::string recvStr(buf);
                        recvStr = recvStr.substr(recvStr.find('^') + 1, recvStr.find('!') - 1);// @Data Server@cpp_dataserver@https://github.com/.../RV-1.0-data_server.git!
                        if (recvStr.length() > 0)
                            repoStrVec.emplace_back(recvStr);
                    }
                    pclose(fp);
                }
                
                for (const auto& i : repoStrVec)// package-list
                {
                    auto splitStr = MyApp::split(i, "@");
                    if (splitStr.size() != 3)
                        continue;
                    repoVec.emplace_back(std::pair<MyApp::Repo, MyApp::ModSign>({{splitStr[0], splitStr[1], splitStr[2]}, {false, false}}));
                }

                // Local module installation status
                repoStrVec.clear();
                fp = fopen((ros2WsDir.path / ".modulesettings").generic_string().c_str(), "r");
                if (fp != NULL)
                {
                    while (fgets(buf, PATH_BUF_SIZE, fp) != NULL)
                    {
                        std::string recvStr(buf);
                        recvStr = recvStr.substr(recvStr.find('#') + 1, recvStr.find('!') - 1);// #cpp_dataserver:eth2:dhcp:true!
                        if (recvStr.length() > 0)
                            repoStrVec.emplace_back(recvStr);
                    }
                    fclose(fp);
                }

                deprecRepoVec.clear();  
                for (const auto& i : repoStrVec)// .modulesettings
                {
                    auto splitStr = MyApp::split(i, ":");
                    if (splitStr.size() != 4)// cpp_dataserver:eth2:dhcp:true
                        continue;
                    bool deprecatedF = true;
                    MyApp::RepoProp prop(splitStr[1], splitStr[2], (splitStr[3] == "true" ? true : false));
                    for (auto& j : repoVec)
                        if (j.first.repoName == splitStr[0])
                        {
                            j.first.prop = prop;
                            j.second = {true, true};
                            deprecatedF = false;
                            break;
                        }
                    if (deprecatedF)
                        deprecRepoVec.emplace_back(std::pair<MyApp::Repo, MyApp::ModSign>({{splitStr[0], splitStr[0], "", prop}, {true, true}}));
                }
                launchCmdStateStr.info("Repo list updated!");
            }// Scan button

            ImGui::SameLine();
            if (ImGui::Button("Install/Remove"))
            {
                installRepoVec.clear();
                removeRepoVec.clear();
                for (const auto& i : repoVec)
                    if (i.second.second)
                        installRepoVec.emplace_back(i.first);
                    else if (i.second.first != i.second.second)
                        removeRepoVec.emplace_back(i.first);
                for (const auto& i : deprecRepoVec)
                    if (!i.second.second)
                        removeRepoVec.emplace_back(i.first);
                ImGui::OpenPopup("Confirm Changes");
            }// Install/Remove button
                

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal("Confirm Changes", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                /*
                 * TODO: add common.yaml change only implement. Considered different version of common.yaml file, 
                 * keep generic properties if possible.
                 */

                // Install repo list
                if (ImGui::IsItemHovered() && ImGui::BeginTooltip())
                {
                    ImGui::PushFont(&contentFont);
                    if (ImGui::BeginTable("showDescription", 2))
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TextColored(repoColorNoChange, "--");
                        ImGui::TableNextColumn();
                        ImGui::TextColored(repoColorNoChange, "No changes");
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TextColored(repoColorInstall, "--");
                        ImGui::TableNextColumn();
                        ImGui::TextColored(repoColorInstall, "Will be Installed");
                        ImGui::TableNextColumn();
                        ImGui::TextColored(repoColorRemove, "--");
                        ImGui::TableNextColumn();
                        ImGui::TextColored(repoColorRemove, "Will be Removed");
                        ImGui::EndTable();
                    }
                    ImGui::PopFont();
                    ImGui::EndTooltip();
                }
                ImGui::SeparatorText("Install");
                ImGui::PushFont(&contentFont);
                for (auto& i : installRepoVec)
                {
                    ImGui::TextColored(repoColorInstall, i.repoDescribe.c_str());
                    if (ImGui::IsItemHovered() && ImGui::BeginTooltip())
                    {

                        ImGui::Text("Interface: %s", i.prop.interface.c_str());
                        ImGui::Text("IP: %s", i.prop.ip.c_str());
                        ImGui::Text("Internet: %s", i.prop.internetRequired ? "yes" : "no");
                        ImGui::EndTooltip();
                    }
                }
                ImGui::PopFont();
                // Remove repo list
                ImGui::SeparatorText("Remove");
                ImGui::PushFont(&contentFont);
                for (auto& i : removeRepoVec)
                {
                    ImGui::TextColored(repoColorRemove, i.repoDescribe.c_str());
                    if (ImGui::IsItemHovered() && ImGui::BeginTooltip())
                    {

                        ImGui::Text("Interface: %s", i.prop.interface.c_str());
                        ImGui::Text("IP: %s", i.prop.ip.c_str());
                        ImGui::Text("Internet: %s", i.prop.internetRequired ? "yes" : "no");
                        ImGui::EndTooltip();
                    }
                }
                ImGui::PopFont();
                // Interface list
                ImGui::SeparatorText("Interfaces");
                ImGui::PushFont(&contentFont);
                for (auto& i : interVec)
                    ImGui::TextColored(repoColorInstall, i.repoDescribe.c_str());
                ImGui::PopFont();
                // Button area
                ImGui::Separator();
                if (ImGui::Button("OK", ImVec2(150, 0)))
                {
                    // Authentication
                    if (localAuth.isConfirmed)
                    {
                        // Run installation
                        char cmdBuf[1024];
                        printf("Create temporary directories...\n");
                        sprintf(cmdBuf, "rm -rf %s", tmpDir.path.generic_string().c_str());
                        system(cmdBuf);
                        sprintf(cmdBuf, "mkdir -p %s", tmpDir.path.generic_string().c_str());
                        system(cmdBuf);
                        printf("Create ROS2 workspace...\n");
                        sprintf(cmdBuf, "mkdir -p %s", ros2SrcDir.path.generic_string().c_str());
                        system(cmdBuf);

                        printf("Update ROS2 workspace package files...\n");
                        for (const auto& i : installRepoVec)
                        {
                            std::string tmpRepoPath = (tmpDir.path / i.repoName).generic_string();
                            std::string wsRepoPath = (ros2SrcDir.path / i.repoName).generic_string();
                            // Clone latest repo
                            sprintf(cmdBuf, "git clone %s %s", i.repoUrl.c_str(), tmpRepoPath.c_str());
                            system(cmdBuf);
                            // Remove current repo
                            sprintf(cmdBuf, "rm -rf %s", wsRepoPath.c_str());
                            system(cmdBuf);
                            // Copy new repo to ROS2 workspace
                            sprintf(cmdBuf, "cp -r %s %s", tmpRepoPath.c_str(), wsRepoPath.c_str());
                            system(cmdBuf);
                            // Create startup files
                            sprintf(cmdBuf, "curl -fsSL %s | bash -s -- -d %s -t %s -p %s -i %s --password %s %s", 
                                ftpGenStartupScript.c_str(), 
                                ros2WsDir.path.generic_string().c_str(), 
                                tmpDir.path.generic_string().c_str(), 
                                i.repoName.c_str(), 
                                i.prop.interface.c_str(), 
                                localAuth.password.c_str(), 
                                i.prop.internetRequired ? "--internet" : "");
                            system(cmdBuf);
                        }

                        for (const auto& i : removeRepoVec)
                        {
                            std::string wsRepoPath = (ros2SrcDir.path / i.repoName).generic_string();
                            // Remove current repo
                            sprintf(cmdBuf, "rm -rf %s", wsRepoPath.c_str());
                            system(cmdBuf);
                            // Remove startup files
                            sprintf(cmdBuf, "curl -fsSL %s | bash -s -- -d %s -t %s -p %s --password %s --remove", 
                                ftpGenStartupScript.c_str(), 
                                ros2WsDir.path.generic_string().c_str(), 
                                tmpDir.path.generic_string().c_str(), 
                                i.repoName.c_str(), 
                                localAuth.password.c_str());
                            system(cmdBuf);
                        }

                        for (const auto& i : interVec)
                        {
                            std::string tmpInterfacePath = (tmpDir.path / i.repoName).generic_string();
                            std::string wsInterfacePath = (ros2SrcDir.path / i.repoName).generic_string();
                            sprintf(cmdBuf, "git clone %s %s", i.repoUrl.c_str(), tmpInterfacePath.c_str());
                            system(cmdBuf);
                            sprintf(cmdBuf, "rm -rf %s", wsInterfacePath.c_str());
                            system(cmdBuf);
                            sprintf(cmdBuf, "cp -r %s %s", tmpInterfacePath.c_str(), wsInterfacePath.c_str());
                            system(cmdBuf);
                        }

                        // Start compile
                        sprintf(cmdBuf, "curl -fsSL %s | bash -s -- -d %s", 
                                ftpCompileScript.c_str(), 
                                ros2WsDir.path.generic_string().c_str());
                        system(cmdBuf);

                        // Generate .modulesettings under ros2WsDir
                        FILE* fp = fopen((ros2WsDir.path / ".modulesettings").generic_string().c_str(), "w");
                        if (fp != NULL)
                        {
                            for (const auto& i : installRepoVec)
                            {
                                fprintf(fp, "#%s:%s:%s:%s!\n", 
                                    i.repoName.c_str(), 
                                    i.prop.interface.c_str(), 
                                    i.prop.ip.c_str(), 
                                    i.prop.internetRequired ? "true" : "false");
                            }
                            fclose(fp);
                        }
                        launchCmdStateStr.info("Install completed! Click Scan to update repo list!");
                    }
                    else
                        launchCmdStateStr.err("Authentication Failed.");
                    // Close pop-up
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(150, 0)))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            
            {// Show Packages
                ImFont labelFont = *ImGui::GetFont();
                // Active Function check-boxes
                ImGui::SeparatorText("Active Functions");
                if (ImGui::BeginTable("activeFunc", 2))
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextColored(style.Colors[ImGuiCol_ButtonHovered], "Packages");
                    ImGui::Separator();
                    ImGui::TableNextColumn();
                    ImGui::TextColored(style.Colors[ImGuiCol_ButtonHovered], "Install/Remove");
                    ImGui::Separator();
                    int cnt = 0;
                    for (auto& i : repoVec)
                    {
                        ImGui::PushID(cnt++);
                        ImGui::PushFont(&contentFont);
                        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();

                        if (!i.second.first)
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
                        if (ImGui::Selectable(i.first.repoDescribe.c_str()))
                        {
                            launchFilePath = ros2SrcDir.path / i.first.repoName / "launch/common.yaml";
                            launchFileF = MyApp::ReadCommonFile(launchFilePath.generic_string().c_str(), launchFile, LAUNCH_CONTENT_SIZE);
                            if (launchFileF)
                                launchCmdStateStr.info("common.yaml found :)");
                            else
                                launchCmdStateStr.info("common.yaml not found :(");
                        }
                        if (!i.second.first)
                            ImGui::PopStyleColor();
                        
                        if (ImGui::BeginPopupContextItem("##"))
                        {
                            ImGui::Text("%s Network Setting", i.first.repoDescribe.c_str());

                            ImGui::BeginGroup();
                            ImGui::BeginGroup();
                            ImGui::Text("Interface: ");
                            ImGui::Text("IP: ");
                            ImGui::EndGroup();

                            ImGui::SameLine();
                            ImGui::BeginGroup();
                            static char interfaceStr[64];
                            strcpy(interfaceStr, i.first.prop.interface.c_str());
                            ImGui::InputText("##interface", interfaceStr, sizeof(interfaceStr));
                            i.first.prop.interface = interfaceStr;

                            static char ipStr[64];
                            strcpy(ipStr, i.first.prop.ip.c_str());
                            ImGui::InputText("##ip", ipStr, sizeof(ipStr));
                            i.first.prop.ip = ipStr;
                            ImGui::EndGroup();
                            ImGui::EndGroup();

                            ImGui::Checkbox("Internet Required", &i.first.prop.internetRequired);

                            ImGui::EndPopup();
                        }
                        ImGui::TableNextColumn();
                        ImGui::Checkbox("##", &i.second.second);
                        ImGui::PopStyleVar();
                        ImGui::PopFont();
                        ImGui::PopID();
                    }
                    ImGui::EndTable();
                }
                ImGui::Separator();

                // Deprecated Function check-boxes
                ImGui::SeparatorText("Deprecated Functions");
                if (ImGui::BeginTable("deprecatedFunc", 2))
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextColored(style.Colors[ImGuiCol_ButtonHovered], "Packages");
                    ImGui::Separator();
                    ImGui::TableNextColumn();
                    ImGui::TextColored(style.Colors[ImGuiCol_ButtonHovered], "Install/Remove");
                    ImGui::Separator();
                    int cnt = 0;
                    for (auto& i : deprecRepoVec)
                    {
                        ImGui::PushID(cnt++);
                        ImGui::PushFont(&contentFont);
                        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        if (ImGui::Selectable(i.first.repoDescribe.c_str()))
                        {
                            launchFilePath = ros2SrcDir.path / i.first.repoName / "launch/common.yaml";
                            launchFileF = MyApp::ReadCommonFile(launchFilePath.generic_string().c_str(), launchFile, LAUNCH_CONTENT_SIZE);
                            if (launchFileF)
                                launchCmdStateStr.info("common.yaml found :)");
                            else
                                launchCmdStateStr.info("common.yaml not found :(");
                        }
                        if (ImGui::BeginPopupContextItem("##"))
                        {
                            ImGui::Text("Deprecated package cannot be configured");
                            ImGui::BeginDisabled();
                            ImGui::Text("%s Network Setting", i.first.repoDescribe.c_str());

                            ImGui::BeginGroup();
                            ImGui::BeginGroup();
                            ImGui::Text("Interface: ");
                            ImGui::Text("IP: ");
                            ImGui::EndGroup();

                            ImGui::SameLine();
                            ImGui::BeginGroup();
                            static char interfaceStr[64];
                            strcpy(interfaceStr, i.first.prop.interface.c_str());
                            ImGui::InputText("##interface", interfaceStr, sizeof(interfaceStr));

                            static char ipStr[64];
                            strcpy(ipStr, i.first.prop.ip.c_str());
                            ImGui::InputText("##ip", ipStr, sizeof(ipStr));
                            ImGui::EndGroup();
                            ImGui::EndGroup();
                            
                            ImGui::Checkbox("Internet Required", &i.first.prop.internetRequired);
                            ImGui::EndDisabled();
                            ImGui::EndPopup();
                        }
                        ImGui::TableNextColumn();
                        ImGui::Checkbox("##", &i.second.second);
                        ImGui::PopStyleVar();
                        ImGui::PopFont();
                        ImGui::PopID();
                    }
                    ImGui::EndTable();
                }
                ImGui::Separator();

                // Interface list
                ImGui::SeparatorText("Interfaces");
                if (ImGui::BeginTable("interfaces", 2))
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextColored(style.Colors[ImGuiCol_ButtonHovered], "Packages");
                    ImGui::Separator();
                    ImGui::TableNextColumn();
                    ImGui::TextColored(style.Colors[ImGuiCol_ButtonHovered], "Install/Remove");
                    ImGui::Separator();
                    int cnt = 0;
                    static bool alwaysEnableF = true;
                    for (auto& i : interVec)
                    {
                        ImGui::PushID(cnt++);
                        ImGui::PushFont(&contentFont);
                        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TextColored(style.Colors[ImGuiCol_NavHighlight], i.repoDescribe.c_str());
                        if (ImGui::IsItemHovered() && ImGui::BeginTooltip())
                        {
                            ImGui::Text("All interfaces must be installed");
                            ImGui::EndTooltip();
                        }

                        ImGui::TableNextColumn();
                        ImGui::BeginDisabled();
                        ImGui::Checkbox("##", &alwaysEnableF);
                        ImGui::EndDisabled();
                        ImGui::PopStyleVar();
                        ImGui::PopFont();
                        ImGui::PopID();
                    }
                    ImGui::EndTable();
                }
                ImGui::Separator();
            }

            ImGui::End();
        }

        // Launch File Configuration Window
        {
            ImGui::Begin("Launch File Configuration");

            ImGui::Text("[Location] ");
            ImGui::SameLine();
            ImGui::PushFont(&contentFont);
            ImGui::Text(launchFilePath.generic_string().c_str());
            ImGui::PopFont();

            ImGui::Separator();
            if (ImGui::Button("Reset"))
            {
                launchFileF = MyApp::ReadCommonFile(launchFilePath.generic_string().c_str(), launchFile, LAUNCH_CONTENT_SIZE);
                if (launchFileF)
                    launchCmdStateStr.info("common.yaml found :)");
                else
                    launchCmdStateStr.info("common.yaml not found :(");
            }

            ImGui::SameLine();
            if (ImGui::Button("Save") && launchFileF)
            {
                FILE* fp = fopen(launchFilePath.generic_string().c_str(), "w");
                if (fp != NULL)
                {
                    fwrite(launchFile, sizeof(char), strlen(launchFile), fp);
                    int res = fclose(fp);
                    if (res == 0)
                        launchCmdStateStr.info("Saved file: " + launchFilePath.generic_string());
                }
            }

            ImGui::SameLine();
            ImGui::Text("[Status] ");
            ImGui::SameLine();
            ImGui::PushFont(&contentFont);
            ImGui::TextColored(launchCmdStateStr.col, launchCmdStateStr.str.c_str());
            ImGui::Separator();

            static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
            ImGui::InputTextMultiline("##source", launchFile, IM_ARRAYSIZE(launchFile), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 128), flags);
            ImGui::PopFont();

            ImGui::End();
        }

        // ImPlot::ShowDemoWindow();
        // ImGui::ShowDemoWindow();



        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
