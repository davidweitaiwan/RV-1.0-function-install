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
#include <thread>

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
    const static size_t PATH_BUF_SIZE = 512;

    // Work directory
    MyApp::WorkspacePath wsPath;
    wsPath.setContentPath(MyApp::GetHomePath() / "rv_content");
    wsPath.setROS2WS(MyApp::GetHomePath() / "ros2_ws");
    wsPath.setRVWS(MyApp::GetHomePath() / "rv_ws");

    // Config file
    std::string rvScriptConfigFile = "script.conf";
    std::map<std::string, std::string> rvScripts;
    bool findScriptF = MyApp::ReadScriptConf(rvScriptConfigFile, wsPath.content->path / "scripts", rvScripts);
    
    // Local network interface
    std::vector<std::string> ifVec;
    bool findIFF = findScriptF ? MyApp::ScanIF(rvScripts["scan-interface"], ifVec) : false;

    // Authentication
    MyApp::AUTH localAuth;
    MyApp::AUTH ftpAuth;
    MyApp::MirrorPath ftpKeyPath("ftp.key");

    // Repository vectors
    std::vector<std::pair<MyApp::Repo, MyApp::ModSign> > repoVec;// {repo, installF}
    std::vector<std::pair<MyApp::Repo, MyApp::ModSign> > deprecRepoVec;// {repo, installF}
    std::vector<MyApp::Repo> installRepoVec;
    std::vector<MyApp::Repo> removeRepoVec;
    std::vector<MyApp::Repo> noChangeRepoVec;

    // Non-ROS repos
    std::vector<MyApp::Repo> installNonROSRepoVec;
    std::vector<MyApp::Repo> removeNonROSRepoVec;

    // Interface vectors
    std::vector<MyApp::Repo> interVec;

    // Color definitions
    ImVec4 repoColorNoChange(1, 1, 1, 1);
    ImVec4 repoColorInstall(0, 1, 0, 1);
    ImVec4 repoColorRemove(1, 0, 0, 1);

    std::vector<ImVec4> repoColorVec = { repoColorNoChange, repoColorInstall, repoColorRemove };

    // Launch file path
    fs::path launchFilePath = "common.yaml";
    const static size_t LAUNCH_CONTENT_SIZE = 1024 * 128;
    static char launchFile[LAUNCH_CONTENT_SIZE];
    bool launchFileF = false;

    MyApp::ColoredString launchCmdStateStr;

    std::thread installRemoveTh;

    // Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to rvScanInterfaceScriptdo a fopen() of the imgui.ini file.
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

        // Menu bar
        {
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("Authentication"))
                {
                    ImGui::PushFont(&contentFont);
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
                    ImGui::PopFont();
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("File"))
                {
                    ImGui::PushFont(&contentFont);
                    ImGui::InputText("Content Path", wsPath.content->c_str, sizeof(wsPath.content->c_str));
                    ImGui::InputText("ROS2 Workspace", wsPath.ros2->c_str, sizeof(wsPath.ros2->c_str));
                    ImGui::InputText("RV Workspace", wsPath.rv->c_str, sizeof(wsPath.rv->c_str));

                    static bool contentChangeF = false;
                    static bool ros2ChangeF = false;
                    static bool rvChangeF = false;
                    if (ImGui::Button("Set"))
                    {
                        contentChangeF = strcmp(wsPath.content->c_str, wsPath.content->path.generic_string().c_str()) != 0;
                        ros2ChangeF = strcmp(wsPath.ros2->c_str, wsPath.ros2->path.generic_string().c_str()) != 0;
                        rvChangeF = strcmp(wsPath.rv->c_str, wsPath.rv->path.generic_string().c_str()) != 0;
                        if (contentChangeF || ros2ChangeF || rvChangeF)
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
                            modColor = contentChangeF ? repoColorInstall : repoColorNoChange;
                            ImGui::TextColored(modColor, wsPath.content->path.generic_string().c_str());
                            ImGui::TableNextColumn();
                            ImGui::TextColored(modColor, " >> ");
                            ImGui::TableNextColumn();
                            ImGui::TextColored(modColor, wsPath.content->c_str);

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            modColor = ros2ChangeF ? repoColorInstall : repoColorNoChange;
                            ImGui::TextColored(modColor, wsPath.ros2->path.generic_string().c_str());
                            ImGui::TableNextColumn();
                            ImGui::TextColored(modColor, " >> ");
                            ImGui::TableNextColumn();
                            ImGui::TextColored(modColor, wsPath.ros2->c_str);

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            modColor = rvChangeF ? repoColorInstall : repoColorNoChange;
                            ImGui::TextColored(modColor, wsPath.rv->path.generic_string().c_str());
                            ImGui::TableNextColumn();
                            ImGui::TextColored(modColor, " >> ");
                            ImGui::TableNextColumn();
                            ImGui::TextColored(modColor, wsPath.rv->c_str);

                            ImGui::EndTable();
                        }

                        ImGui::Separator();
                        static bool fileRemoveF = true;
                        ImGui::BeginDisabled();
                        ImGui::Checkbox("Remove Current Files", &fileRemoveF);
                        ImGui::EndDisabled();
                        if (ImGui::Button("OK", ImVec2(150, 0)))
                        {
                            if (localAuth.isConfirmed)
                            {
                                // Remove current installed packages under ROS2 workspace
                                if (ros2ChangeF)
                                {
                                    auto vec = MyApp::ScanLocalPackages(wsPath.ros2->path, ifVec);
                                    char buf[PATH_BUF_SIZE];
                                    for (const auto& i : vec)
                                    {
                                        // Remove startup files
                                        sprintf(buf, "bash %s -d %s -p %s --password %s --remove", 
                                            rvScripts["generate-startup"].c_str(), 
                                            wsPath.ros2->path.generic_string().c_str(), 
                                            i.repoName.c_str(), 
                                            localAuth.password.c_str());
                                        system(buf);
                                    }

                                    // Remove current workspace
                                    sprintf(buf, "sudo rm -rf %s", wsPath.ros2->path.generic_string().c_str());
                                    system(buf);

                                    // Update path
                                    wsPath.ros2->updatePath();
                                    wsPath.ros2Src->update(wsPath.ros2->path / "src");
                                    
                                    // Create new workspace
                                    sprintf(buf, "mkdir -p %s", wsPath.ros2Src->c_str);
                                    system(buf);

                                }

                                // Remove current installed packages under RV workspace
                                if (rvChangeF)
                                {
                                    auto vec = MyApp::ScanLocalPackages(wsPath.rv->path, ifVec);
                                    char buf[PATH_BUF_SIZE];
                                    for (const auto& i : vec)
                                    {
                                        // Remove startup files
                                        sprintf(buf, "bash %s -d %s -p %s --password %s --remove --no-ros", 
                                            rvScripts["generate-startup"].c_str(), 
                                            wsPath.rv->path.generic_string().c_str(), 
                                            i.repoName.c_str(), 
                                            localAuth.password.c_str());
                                        system(buf);
                                    }

                                    // Remove current workspace
                                    sprintf(buf, "sudo rm -rf %s", wsPath.rv->path.generic_string().c_str());
                                    system(buf);

                                    // Update path
                                    wsPath.rv->updatePath();
                                    wsPath.rvSrc->update(wsPath.rv->path / "src");
                                    
                                    // Create new workspace
                                    sprintf(buf, "mkdir -p %s", wsPath.rvSrc->c_str);
                                    system(buf);
                                }

                                // Move files to new directory
                                if (contentChangeF)
                                {
                                    char buf[PATH_BUF_SIZE];
                                    sprintf(buf, "mv %s %s", wsPath.content->path.generic_string().c_str(), wsPath.content->c_str);
                                    system(buf);
                                    wsPath.content->updatePath();

                                    findScriptF = MyApp::ReadScriptConf(rvScriptConfigFile, wsPath.content->path / "scripts", rvScripts);
                                }

                                launchCmdStateStr.info("Path Changed!");
                            }
                            else
                                printf("Authentication Failed! File not removed.\n");
                            
                            contentChangeF = false;
                            ros2ChangeF = false;
                            rvChangeF = false;
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();;
                        if (ImGui::Button("Cancel", ImVec2(150, 0)))
                        {
                            wsPath.content->updateStr();
                            wsPath.ros2->updateStr();
                            wsPath.rv->updateStr();

                            contentChangeF = false;
                            ros2ChangeF = false;
                            rvChangeF = false;
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Reset"))
                    {
                        wsPath.content->updateStr();
                        wsPath.ros2->updateStr();
                        wsPath.rv->updateStr();

                        contentChangeF = false;
                        ros2ChangeF = false;
                        rvChangeF = false;
                    }
                    // Remove button
                    ImGui::Separator();
                    if (ImGui::Button("Remove Installed Files"))
                    {
                        if (localAuth.isConfirmed)
                        {
                            // ROS2 workspace
                            {
                                auto vec = MyApp::ScanLocalPackages(wsPath.ros2->path, ifVec);
                                char buf[PATH_BUF_SIZE];
                                for (const auto& i : vec)
                                {
                                    // Remove startup files
                                    sprintf(buf, "bash %s -d %s -t %s -p %s --password %s --remove", 
                                        rvScripts["generate-startup"].c_str(), 
                                        wsPath.ros2->path.generic_string().c_str(), 
                                        wsPath.content->path.generic_string().c_str(), 
                                        i.repoName.c_str(), 
                                        localAuth.password.c_str());
                                    system(buf);
                                }
                                sprintf(buf, "sudo rm -rf %s", wsPath.ros2->path.generic_string().c_str());
                                system(buf);

                                sprintf(buf, "mkdir -p %s", wsPath.ros2Src->path.generic_string().c_str());
                                system(buf);
                            }

                            // RV workspace
                            {
                                auto vec = MyApp::ScanLocalPackages(wsPath.rv->path, ifVec);
                                char buf[PATH_BUF_SIZE];
                                for (const auto& i : vec)
                                {
                                    // Remove startup files
                                    sprintf(buf, "bash %s -d %s -t %s -p %s --password %s --remove --no-ros", 
                                        rvScripts["generate-startup"].c_str(), 
                                        wsPath.rv->path.generic_string().c_str(), 
                                        wsPath.content->path.generic_string().c_str(), 
                                        i.repoName.c_str(), 
                                        localAuth.password.c_str());
                                    system(buf);
                                }
                                sprintf(buf, "sudo rm -rf %s", wsPath.rv->path.generic_string().c_str());
                                system(buf);

                                sprintf(buf, "mkdir -p %s", wsPath.rvSrc->path.generic_string().c_str());
                                system(buf);
                            }
                        }
                        else
                            launchCmdStateStr.err("Authentication Failed!");
                    }
                    if (ImGui::Button("Remove Content"))
                    {
                        if (localAuth.isConfirmed)
                        {
                            char buf[PATH_BUF_SIZE];
                            sprintf(buf, "sudo rm -rf %s", wsPath.content->path.generic_string().c_str());
                            system(buf);

                            sprintf(buf, "mkdir -p %s", wsPath.content->path.generic_string().c_str());
                            system(buf);

                            findScriptF = false;
                        }
                        else
                            launchCmdStateStr.err("Authentication Failed!");
                    }
                    ImGui::PopFont();
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Update"))
                {
                    ImGui::PushFont(&contentFont);
                    if (ImGui::Button("Scan"))
                        findScriptF = MyApp::ReadScriptConf(rvScriptConfigFile, wsPath.content->path / "scripts", rvScripts);
                    ImGui::SameLine();
                    if (findScriptF)
                        ImGui::TextColored(repoColorInstall, "Scripts found.");
                    else
                        ImGui::TextColored(repoColorRemove, "Scripts not found.");

                    if (ImGui::Button("Scan"))
                        findIFF = MyApp::ScanIF(rvScripts["scan-interface"], ifVec);
                    ImGui::SameLine();
                    if (findIFF)
                        ImGui::TextColored(repoColorInstall, "Network IF found.");
                    else
                        ImGui::TextColored(repoColorRemove, "Network IF not found.");
                    ImGui::PopFont();
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Help"))
                {
                    ImGui::PushFont(&contentFont);
                    ImGui::Text("Version: ");
                    ImGui::SameLine();
                    ImGui::TextColored(repoColorInstall, "%3.3f", VCU_INSTALL_VERSION);
                    ImGui::PopFont();
                    ImGui::EndMenu();
                }

                ImGui::EndMainMenuBar();
            }
        }// Menu bar

        // Repo Window
        {
            if (!findScriptF || !findIFF)
                ImGui::BeginDisabled();

            ImGui::Begin("Package Management");

            if (ImGui::Button("Scan"))// TODO: functionalize. Add non-ROS2 function installation.
            {
                if (localAuth.isConfirmed)
                {
                    interVec.clear();
                    repoVec.clear();
                    deprecRepoVec.clear();

                    char buf[PATH_BUF_SIZE];
                    char cmdBuf[PATH_BUF_SIZE];

                    // Create content directory if not exists
                    sprintf(cmdBuf, "mkdir -p %s", wsPath.content->path.generic_string().c_str());
                    system(cmdBuf);
                    
                    std::vector<std::string> repoStrVec;

                    // Online interface list check
                    sprintf(cmdBuf, "cat %s", rvScripts["interface-list"].c_str());
                    FILE* fp = popen(cmdBuf, "r");
                    if (fp != NULL)
                    {
                        while (fgets(buf, PATH_BUF_SIZE, fp) != NULL)
                        {
                            std::string recvStr(buf);
                            std::string repoStr = recvStr.substr(recvStr.find('^') + 1, recvStr.rfind('!') - 1);// ^Vehicle Interfaces@vehicle_interfaces@https://github.com/.../RV-1.0-vehicle_interfaces.git!
                            if (repoStr.length() > 0 && recvStr != repoStr)
                                repoStrVec.emplace_back(repoStr);
                        }
                        pclose(fp);
                    }
                    
                    for (const auto& i : repoStrVec)// interface-list
                    {
                        auto splitStr = MyApp::split(i, "@");
                        if (splitStr.size() != 3)
                            continue;
                        interVec.emplace_back(splitStr[0], splitStr[1], splitStr[2], ifVec);
                        
                        // Check repo under content. Download new repo if content repo not found.
                        std::string _repoUrl = interVec.back().repoUrl;
                        std::string _repoName = interVec.back().repoName;
                        fs::path _repoContentDir = wsPath.content->path / _repoName;
                        if (!fs::exists(_repoContentDir / ".git"))
                        {
                            sprintf(cmdBuf, "rm -rf %s", _repoContentDir.generic_string().c_str());
                            system(cmdBuf);

                            sprintf(cmdBuf, "git clone \"%s\" \"%s\" -q", _repoUrl.c_str(), _repoContentDir.generic_string().c_str());
                            system(cmdBuf);
                            
                            if (!fs::exists(_repoContentDir / ".git"))
                            {
                                printf("Clone package %s (%s) error. Ignored...\n", _repoName.c_str(), _repoUrl.c_str());
                                interVec.pop_back();
                                continue;
                            }
                            printf("Clone %s package under %s\n", _repoName.c_str(), _repoContentDir.generic_string().c_str());
                        }

                        // Check repo branch.
                        interVec.back().repoBranchVec = MyApp::CheckRepoBranch(_repoContentDir);
                        if (interVec.back().repoBranchVec.size() <= 0)
                        {
                            printf("Get branch error: package %s. Ignored...\n", _repoName.c_str());
                            interVec.pop_back();
                            continue;
                        }
                    }

                    // Online module list check
                    repoStrVec.clear();
                    sprintf(cmdBuf, "cat %s", rvScripts["package-list"].c_str());
                    fp = popen(cmdBuf, "r");
                    if (fp != NULL)
                    {
                        while (fgets(buf, PATH_BUF_SIZE, fp) != NULL)
                        {
                            std::string recvStr(buf);
                            std::string repoStr = recvStr.substr(recvStr.find('^') + 1, recvStr.rfind('!') - 1);// ^Data Server@cpp_dataserver@https://github.com/.../RV-1.0-data_server.git!
                            if (repoStr.length() > 0 && recvStr != repoStr)
                                repoStrVec.emplace_back(repoStr);
                        }
                        pclose(fp);
                    }
                    
                    for (const auto& i : repoStrVec)// package-list
                    {
                        auto splitStr = MyApp::split(i, "@");
                        if (splitStr.size() != 3)
                            continue;
                        repoVec.emplace_back(std::pair<MyApp::Repo, MyApp::ModSign>({{splitStr[0], splitStr[1], splitStr[2], ifVec}, {false, false}}));

                        // Check repo under content. Download new repo if content repo not found.
                        std::string _repoUrl = repoVec.back().first.repoUrl;
                        std::string _repoName = repoVec.back().first.repoName;
                        fs::path _repoContentDir = wsPath.content->path / _repoName;
                        if (!fs::exists(_repoContentDir / ".git"))
                        {
                            sprintf(cmdBuf, "rm -rf %s", _repoContentDir.generic_string().c_str());
                            system(cmdBuf);

                            sprintf(cmdBuf, "git clone \"%s\" \"%s\" -q", _repoUrl.c_str(), _repoContentDir.generic_string().c_str());
                            system(cmdBuf);
                            
                            if (!fs::exists(_repoContentDir / ".git"))
                            {
                                printf("Clone package %s (%s) error. Ignored...\n", _repoName.c_str(), _repoUrl.c_str());
                                repoVec.pop_back();
                                continue;
                            }
                            printf("Clone %s package under %s\n", _repoName.c_str(), _repoContentDir.generic_string().c_str());
                        }

                        // Check repo branch.
                        repoVec.back().first.repoBranchVec = MyApp::CheckRepoBranch(_repoContentDir);
                        if (repoVec.back().first.repoBranchVec.size() <= 0)
                        {
                            printf("Get branch error: package %s. Ignored...\n", _repoName.c_str());
                            repoVec.pop_back();
                            continue;
                        }

                        // Check ROS2 support
                        repoVec.back().first.nonROS = !MyApp::CheckROSSupport(_repoContentDir);
                    }

                    // Local module installation status
                    repoStrVec.clear();
                    fp = fopen((wsPath.ros2->path / ".modulesettings").generic_string().c_str(), "r");
                    if (fp != NULL)
                    {
                        while (fgets(buf, PATH_BUF_SIZE, fp) != NULL)
                        {
                            std::string recvStr(buf);
                            std::string repoStr = recvStr.substr(recvStr.find('#') + 1, recvStr.rfind('!') - 1);// #cpp_dataserver:master:eth2:dhcp:true:false!
                            if (repoStr.length() > 0 && recvStr != repoStr)
                                repoStrVec.emplace_back(repoStr);
                        }
                        fclose(fp);
                    }

                    for (const auto& i : repoStrVec)// .modulesettings
                    {
                        auto splitStr = MyApp::split(i, ":");
                        if (splitStr.size() != 6)// cpp_dataserver:master:eth2:dhcp:true:false
                            continue;
                        bool deprecatedF = true;
                        MyApp::RepoNetworkProp prop(splitStr[2], splitStr[3], splitStr[4] == "true", ifVec);

                        {
                            // Check network interface deprecated
                            bool isDeprecF = true;
                            for (int niIdx = 0; niIdx < ifVec.size(); niIdx++)
                                if (prop.interface == ifVec[niIdx])
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

                        for (auto& [repo, modSign] : repoVec)
                            if (repo.repoName == splitStr[0])
                            {
                                repo.prop = prop;
                                repo.repoBranch = splitStr[1];
                                repo.nonROS = splitStr[5] == "true";

                                {
                                    // Check branch deprecated
                                    bool isDeprecF = true;
                                    for (int bIdx = 0; bIdx < repo.repoBranchVec.size(); bIdx++)
                                        if (repo.repoBranch == repo.repoBranchVec[bIdx])
                                        {
                                            repo.repoBranchIdx = bIdx;
                                            isDeprecF = false;
                                            break;
                                        }
                                    if (isDeprecF)
                                    {
                                        repo.repoBranch += " (deprecated)";
                                        repo.repoBranchVec.push_back(repo.repoBranch);
                                        repo.repoBranchIdx = repo.repoBranchVec.size() - 1;
                                        repo.repoBranchDeprecIdx = repo.repoBranchIdx;
                                    }
                                }

                                modSign = {true, true};
                                deprecatedF = false;
                                break;
                            }
                        if (deprecatedF)
                        {
                            deprecRepoVec.emplace_back(std::pair<MyApp::Repo, MyApp::ModSign>({{splitStr[0], splitStr[0], "", prop}, {true, true}}));
                            deprecRepoVec.back().first.repoBranch = splitStr[1];
                            deprecRepoVec.back().first.nonROS = splitStr[5] == "true";
                        }
                    }
                    launchCmdStateStr.info("Repo list updated!");
                }
                else
                    launchCmdStateStr.err("Authentication Failed.");
            }// Scan button

            ImGui::SameLine();
            if (ImGui::Button("Install/Remove"))
            {
                // Authentication
                if (localAuth.isConfirmed)
                {
                    installRepoVec.clear();
                    removeRepoVec.clear();
                    noChangeRepoVec.clear();

                    installNonROSRepoVec.clear();
                    removeNonROSRepoVec.clear();
                    // TODO: Add non-ROS method

                    for (const auto& [repo, modSign] : repoVec)
                        if (modSign.second)
                        {
                            if (repo.repoBranchDeprecIdx < 0 && repo.prop.interfaceDeprecIdx < 0 && repo.prop.interface != "NONE")
                            {
                                if (repo.nonROS)
                                    installNonROSRepoVec.emplace_back(repo);
                                else
                                    installRepoVec.emplace_back(repo);
                            }
                        }
                        else if (modSign.first != modSign.second)
                        {
                            if (repo.nonROS)
                                removeNonROSRepoVec.emplace_back(repo);
                            else
                                removeRepoVec.emplace_back(repo);
                        }
                    for (const auto& [repo, modSign] : deprecRepoVec)
                        if (!modSign.second)
                        {
                            if (repo.nonROS)
                                removeNonROSRepoVec.emplace_back(repo);
                            else
                                removeRepoVec.emplace_back(repo);
                        }
                        else
                            noChangeRepoVec.emplace_back(repo);
                    ImGui::OpenPopup("Confirm Changes");
                }
                else
                    launchCmdStateStr.err("Authentication Failed.");
            }// Install/Remove button
                

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal("Confirm Changes", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                /**
                 * TODO: add common.yaml change only implementation. Considering different version of common.yaml file, 
                 * keep generic properties if possible.
                */

                static bool installF = false;
                static bool procF = false;
                static int colorCnt = 0;

                if (installF)
                    ImGui::BeginDisabled();

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
                if (ImGui::BeginTable("installList", 2))
                {
                    for (auto& i : installRepoVec)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TextColored(repoColorInstall, i.repoDescribe.c_str());
                        if (ImGui::IsItemHovered() && ImGui::BeginTooltip())
                        {
                            ImGui::Text("Interface: %s", i.prop.interface.c_str());
                            ImGui::Text("IP: %s", i.prop.ip.c_str());
                            ImGui::Text("Internet: %s", i.prop.internetRequired ? "yes" : "no");
                            ImGui::EndTooltip();
                        }
                        ImGui::TableNextColumn();
                        ImGui::TextColored(repoColorInstall, ("[" + i.repoBranch + "]").c_str());
                    }
                    for (auto& i : installNonROSRepoVec)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TextColored(repoColorInstall, i.repoDescribe.c_str());
                        if (ImGui::IsItemHovered() && ImGui::BeginTooltip())
                        {
                            ImGui::Text("Interface: %s", i.prop.interface.c_str());
                            ImGui::Text("IP: %s", i.prop.ip.c_str());
                            ImGui::Text("Internet: %s", i.prop.internetRequired ? "yes" : "no");
                            ImGui::EndTooltip();
                        }
                        ImGui::TableNextColumn();
                        ImGui::TextColored(repoColorInstall, ("[" + i.repoBranch + "]").c_str());
                    }
                    ImGui::EndTable();
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
                for (auto& i : removeNonROSRepoVec)
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
                if (ImGui::BeginTable("interfaceList", 2))
                {
                    for (auto& i : interVec)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TextColored(repoColorInstall, i.repoDescribe.c_str());
                        ImGui::TableNextColumn();
                        ImGui::TextColored(repoColorInstall, ("[" + i.repoBranch + "]").c_str());
                    }
                    ImGui::EndTable();
                }
                ImGui::PopFont();

                // Remove repo list
                ImGui::SeparatorText("No Changes");
                ImGui::PushFont(&contentFont);
                for (auto& i : noChangeRepoVec)
                {
                    ImGui::TextColored(repoColorNoChange, i.repoDescribe.c_str());
                    if (ImGui::IsItemHovered() && ImGui::BeginTooltip())
                    {
                        ImGui::Text("Interface: %s", i.prop.interface.c_str());
                        ImGui::Text("IP: %s", i.prop.ip.c_str());
                        ImGui::Text("Internet: %s", i.prop.internetRequired ? "yes" : "no");
                        ImGui::EndTooltip();
                    }
                }
                ImGui::PopFont();

                // Button area
                ImGui::Separator();

                if (installF)
                    ImGui::PushStyleColor(ImGuiCol_Button, repoColorVec[colorCnt++ % 2]);

                if (ImGui::Button("OK", ImVec2(150, 0)))
                {
                    // Authentication
                    if (localAuth.isConfirmed)
                    {
                        installRemoveTh = std::thread(MyApp::RunInstallRemove, wsPath, rvScripts, installRepoVec, removeRepoVec, interVec, installNonROSRepoVec, removeNonROSRepoVec, noChangeRepoVec, localAuth, std::ref(procF));
                    }
                    else
                    {
                        launchCmdStateStr.err("Authentication Failed.");
                        ImGui::CloseCurrentPopup();
                    }
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(150, 0)))
                {
                    ImGui::CloseCurrentPopup();
                }

                if (installF)
                    ImGui::PopStyleColor(1);

                if (installF)
                    ImGui::EndDisabled();

                if (procF)
                {
                    installF = true;
                    std::this_thread::sleep_for(500ms);
                }
                else
                {
                    if (installF)
                    {
                        installF = false;
                        procF = false;
                        colorCnt = 0;
                        installRemoveTh.join();
                        launchCmdStateStr.succ("Install completed! Click Scan to update repo list!");
                        ImGui::CloseCurrentPopup();
                    }
                    installF = false;
                }

                ImGui::EndPopup();
            }

            
            {// Show Packages
                ImFont labelFont = *ImGui::GetFont();
                // Active Function check-boxes
                ImGui::SeparatorText("Active Packages");
                if (ImGui::BeginTable("activePkgTable", 3))
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextColored(style.Colors[ImGuiCol_ButtonHovered], "Packages");
                    ImGui::Separator();
                    ImGui::TableNextColumn();
                    ImGui::TextColored(style.Colors[ImGuiCol_ButtonHovered], "Branch");
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

                        ImGui::TableNextColumn();// Packages

                        if (!i.second.first)
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
                        if (ImGui::Selectable(i.first.repoDescribe.c_str()))
                        {
                            launchFilePath = wsPath.ros2Src->path / i.first.repoName / "launch/common.yaml";
                            launchFileF = MyApp::ReadCommonFile(launchFilePath.generic_string().c_str(), launchFile, LAUNCH_CONTENT_SIZE);
                            if (launchFileF)
                                launchCmdStateStr.info("common.yaml found :)");
                            else
                                launchCmdStateStr.info("common.yaml not found :(");
                        }
                        if (!i.second.first)
                            ImGui::PopStyleColor();
                        
                        if (ImGui::BeginPopupContextItem("##activePkgTablePopup"))
                        {
                            ImGui::Text("%s Network Setting", i.first.repoDescribe.c_str());

                            ImGui::BeginGroup();
                            ImGui::BeginGroup();
                            ImGui::Text("Interface: ");
                            ImGui::Text("IP: ");
                            ImGui::EndGroup();

                            ImGui::SameLine();
                            ImGui::BeginGroup();
                            ImGui::Combo("##activeInterfaceCombo", &i.first.prop.interfaceIdx, MyApp::StrVecToCStrArr(i.first.prop.interfaceVec), i.first.prop.interfaceVec.size());
                            i.first.prop.interface = i.first.prop.interfaceVec[i.first.prop.interfaceIdx];

                            static char ipStr[64];
                            strcpy(ipStr, i.first.prop.ip.c_str());
                            ImGui::InputText("##ip", ipStr, sizeof(ipStr));
                            i.first.prop.ip = ipStr;
                            ImGui::EndGroup();
                            ImGui::EndGroup();

                            ImGui::Checkbox("Internet Required", &i.first.prop.internetRequired);

                            ImGui::EndPopup();
                        }
                        ImGui::TableNextColumn();// Branch
                        ImGui::Combo("##activePkgTableCombo", &i.first.repoBranchIdx, MyApp::StrVecToCStrArr(i.first.repoBranchVec), i.first.repoBranchVec.size());
                        i.first.repoBranch = i.first.repoBranchVec[i.first.repoBranchIdx];

                        ImGui::TableNextColumn();// Install/Remove
                        ImGui::Checkbox("##activePkgTableCheckbox", &i.second.second);
                        ImGui::PopStyleVar();
                        ImGui::PopFont();
                        ImGui::PopID();
                    }
                    ImGui::EndTable();
                }
                ImGui::Separator();

                // Deprecated Function check-boxes
                ImGui::SeparatorText("Deprecated Packages");
                if (ImGui::BeginTable("deprecatedPkgTable", 3))
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextColored(style.Colors[ImGuiCol_ButtonHovered], "Packages");
                    ImGui::Separator();
                    ImGui::TableNextColumn();
                    ImGui::TextColored(style.Colors[ImGuiCol_ButtonHovered], "Branch");
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
                            launchFilePath = wsPath.ros2Src->path / i.first.repoName / "launch/common.yaml";
                            launchFileF = MyApp::ReadCommonFile(launchFilePath.generic_string().c_str(), launchFile, LAUNCH_CONTENT_SIZE);
                            if (launchFileF)
                                launchCmdStateStr.info("common.yaml found :)");
                            else
                                launchCmdStateStr.info("common.yaml not found :(");
                        }
                        if (ImGui::BeginPopupContextItem("##deprecatedPkgTablePopup"))
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
                        ImGui::Text(i.first.repoBranch.c_str());
                        ImGui::TableNextColumn();
                        ImGui::Checkbox("##deprecatedPkgTableCheckbox", &i.second.second);
                        ImGui::PopStyleVar();
                        ImGui::PopFont();
                        ImGui::PopID();
                    }
                    ImGui::EndTable();
                }
                ImGui::Separator();

                // Interface list
                ImGui::SeparatorText("Interfaces");
                if (ImGui::BeginTable("interfaces", 3))
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextColored(style.Colors[ImGuiCol_ButtonHovered], "Packages");
                    ImGui::Separator();
                    ImGui::TableNextColumn();
                    ImGui::TextColored(style.Colors[ImGuiCol_ButtonHovered], "Branch");
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

                        ImGui::TableNextColumn();// Branch
                        ImGui::Combo("##interfacesCombo", &i.repoBranchIdx, MyApp::StrVecToCStrArr(i.repoBranchVec), i.repoBranchVec.size());
                        i.repoBranch = i.repoBranchVec[i.repoBranchIdx];

                        ImGui::TableNextColumn();
                        ImGui::BeginDisabled();
                        ImGui::Checkbox("##interfacesCheckbox", &alwaysEnableF);
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

            if (!findScriptF || !findIFF)
                ImGui::EndDisabled();
        }// Repo Window

        // Launch File Configuration Window
        {
            if (!findScriptF || !findIFF)
                ImGui::BeginDisabled();

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

            ImVec2 contentSz = ImGui::GetContentRegionAvail();

            static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
            ImGui::InputTextMultiline("##source", launchFile, IM_ARRAYSIZE(launchFile), contentSz, flags);
            ImGui::PopFont();

            ImGui::End();

            if (!findScriptF || !findIFF)
                ImGui::EndDisabled();
        }// Launch File Configuration Window

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
