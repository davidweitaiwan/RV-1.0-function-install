# Robot Vehicle 1.0 VCU Descriptions

Web version: https://hackmd.io/@cocobird231/ry6ywETt3

*`Updated: 2023/09/21`*

:::warning
- **Prerequisite**

The VCU installer can be easily deployed by running pre-install script. The requirement of the pre-install script:
> - OS: `Ubuntu 18.04` up to `Ubuntu 22.04`
> - Compiler: `g++-7` up to `g++-11`
> - ROS2 (the pre-install script will not install automatically)

The pre-install script will automatically installed the `git`, `libglfw3-dev` and `nlohmann-json3-dev` packages. For `Ubuntu 18.04`, the third-party PPA `team-xbmc` will be added for `nlohmann-json3-dev` support.

If the system environment did not fit the requirements, then build the VCU installer manually. The program were based on C++17 and Dear ImGui with OpenGL and GLFW platform. The following dependencies were required: 
- `libglfw3-dev`
- `nlohmann-json3-dev`
:::

## Quick Deployment
The VCU installer can be easily deployed by running pre-install script. There are **two** ways to run the pre-install script: 
1. **Run the pre-install script `get-vcu-install.sh` manually**
    ```bash
    . get-vcu-install.sh
    ```
2. **Run the pre-install script using `curl`**
    ```bash
    curl -fsSL ftp://61.220.23.239/rv-12/get-vcu-install.sh | bash
    ```
The new directory `rv-vcu-install` will be created under `$HOME`.

## Build Manually
:::warning
The following method only tested under Ubuntu 18.04, Ubuntu 20.04 and Ubuntu 22.04 based-on x64 and aarch64 platform.
:::

### Get Repo
1. Clone repo
```bash
git clone https://github.com/davidweitaiwan/RV-1.0-function-install.git rv-vcu-install
```
2. Get into the dorectory
```bash
cd rv-vcu-install
```

### g++ build
- **For Ubuntu 22.04 x64 System**
    ```bash
    # Build (using g++-11 compiler)
    g++ -std=c++17 -O3 *.cpp -o vcu-install -lGL -lglfw -lpthread
    ```
- **For Ubuntu 20.04 aarch64 System**
    ```bash
    # Build (using g++-9 compiler)
    g++ -std=c++17 -O3 *.cpp -o vcu-install -lGL -lglfw -ldl -lpthread
    ```
- **For Ubuntu 18.04 aarch64 System**
    ```bash
    # Build (using g++-7 compiler)
    g++ -std=c++17 -O3 *.cpp -o vcu-install -lGL -lglfw -ldl -lstdc++fs -lpthread
    ```

### Allow Execution
```bash
sudo chmod a+x vcu-install
```

### Download Configure Files
The configure files and `vcu-install` must be put in same directory.
- **`script.conf` for scripts downloading (required)**
    ```bash
    wget -O script.conf ftp://61.220.23.239/rv-12/vcu-install/script.conf
    ```
- `imgui.ini` for `vcu-install` window setting (not necessary)
    ```bash
    wget -O imgui.ini ftp://61.220.23.239/rv-12/vcu-install/imgui.ini
    ```

## Usage
Double click `vcu-install` or run with command `./vcu-install` **(recommended)** to open GUI installer. Under the GUI, the window separated as two main part, the right part work as the package manager, and the left part is the configure file setting area.
![](https://hackmd.io/_uploads/S1duAoF1p.png)


### Authentication
On the top menu bar, Click `Authentication` menu to check password and FTP authentication. If the authentication passed, the status will turn green.
![](https://hackmd.io/_uploads/HyP90oYkp.png)
![](https://hackmd.io/_uploads/ryOoAiFJa.png)
![](https://hackmd.io/_uploads/BkPnAsF16.png)

### Package Management
On the right side, Click `Scan` button will get the install-available package list, and search the current installed package under ROS2 workspace.

The color green shows the package is available but local workspace not installed; The color white shows the package installed under workspace. If the installed package does not appear on the install-available package list, then the package will be considered as deprecated package.
![](https://hackmd.io/_uploads/SygBYbnty6.png)

The `Install/Remove` check box determines whether the package need to be install or remove. The `Branch` combo box determines the package install version.

Right click the package will show up the network configuration window, including network interface and IP setup. The network interface can be selected from combo box which installer automatically detected at start-up.
![](https://hackmd.io/_uploads/rkMEm2Kk6.png)

While all package set up correctly, click `Install/Remove` button to confirm changes, then click `OK` to start installation. While in the process, the `OK` and `Cancel` button will blinking until process complete.
![](https://hackmd.io/_uploads/SJDUBhF1a.png)
![](https://hackmd.io/_uploads/Hy2uH2K1p.png)

While install completed, click `Scan` button again to update package manager window. The installed package will be shown as white.
![](https://hackmd.io/_uploads/Syo7whtJa.png)

### Launch File Configuration
The launch file determines the package behaivor during program launch time, including node name, topic name, service setting, etc..

The `Launch File Configuration` window shows the launch file edit area, for installed package shown as white at right side of window, left-click the package name to show the content of launch file. While complete the configuration, click the `Save` button to save the launch file.

### Change Working Directory (Optional)
On the top menu bar, Click `File` menu to set the path of temporary directory and ROS2 workspace. For normal use, keep default. Users can also clean up the temporary file while install finished, or remove whole installed packages. For the package removal, it is not safe to manually delete the ROS2 workspace cause the start-up files and the rest of configures were still exist.
![](https://hackmd.io/_uploads/r1vdunYkp.png)

### Update Scripts and Network Interface (Optional)
If the `script.conf` file not found or content incorrect, the installer will be disabled except the `Update` buttons. Click `Scan` to retrieve the latest scripts.

If network interface not found or modified, click `Scan` to get latest interfaces.
![](https://hackmd.io/_uploads/ryPq5hYyT.png)

