# Robot Vehicle 1.0 Function Descriptions

###### tags: `Robot Vehicle`

*`Updated: 2023/07/16`*

## Quick Installation
The robot vehicle functions can be easily installed by following two method:
1. **Run `curl`**
```
curl -fsSL https://raw.githubusercontent.com/davidweitaiwan/RV-1.0-function-install/master/scripts/get-function-install.sh | bash
```
The GUI installer will be generated at `$HOME/rv-function-install`
2. **Run `get-function-install.sh` Manually**
```
. get-function-install.sh
```
The GUI installer will be generated at `$HOME/rv-function-install`

## Usage
Double click `function-install` or run with command `./function-install` (recommended) to open GUI installer. Under the GUI, the window separated as two main part, the right part work as the package manager, and the left part is the configure file setting area.
![](https://hackmd.io/_uploads/Sko85BaKh.png)


### Authentication
On the top menu bar, Click `Authentication` menu to check password and FTP authentication. If the authentication passed, the status will turn green.
![](https://hackmd.io/_uploads/S13KqH6t2.png)
![](https://hackmd.io/_uploads/rkg9qBpFn.png)
![](https://hackmd.io/_uploads/BJQ9qHaK2.png)
![](https://hackmd.io/_uploads/ry3s9BaF2.png)


### Package Management
On the right side, Click `Scan` button will get the install available packages from server, and search the current installed package under ROS2 workspace.

The color green shows the package is available but local workspace not installed; The color white shows the package was installed under workspace. If local package was installed but not listed from server, then the packages is considered as deprecated package.
![](https://hackmd.io/_uploads/rkv46SaKh.png)

The `Install/Remove` check box determines whether the package need to be install or remove.
![](https://hackmd.io/_uploads/Hk0jgIpK2.png)


Right click the package will show up the network configuration window. It's necessary for correct interface setting and internet requirement set up.
![](https://hackmd.io/_uploads/ryFteUaK3.png)

While all package set up correctly, click `Install/Remove` button to confirm changes, then click `OK` to start installation.
![](https://hackmd.io/_uploads/HkX0x8aY3.png)

While installation completed, click `Scan` button again to update package manager window. The installed package will shown as white.
![](https://hackmd.io/_uploads/rJg0b8aF3.png)

### Launch File Setting
The launch file determines the package behaivor during program launch time, including node name, topic name, service setting, etc..

The left side of window shows the launch file edit area, for installed package shown as white at right side of window, left-click the package name to show the content of launch file. While complete the configuration, click the `Save` button to save the launch file.
![](https://hackmd.io/_uploads/H1BENU6Yh.png)


### Installation Path (Optional)
On the top menu bar, Click `File` menu to set the path of temporary directory and ROS2 workspace. For normal use, keep default. Users can also clean up the temporary file while finished install, or remove whole installed packages. It's not safe for manually delete the ROS2 workspace, cause the start-up files and the rest of configures were still exist.
![](https://hackmd.io/_uploads/H1D-VLpF3.png)
