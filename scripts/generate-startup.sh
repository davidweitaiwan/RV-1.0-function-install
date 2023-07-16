#!/usr/bin/bash
ros2_ws_dir="$HOME/ros2_ws"
temp_dir="."
package_name="NONE"
interface="NONE"
internet_required="False"
remove_startup="False"
pswd=""
while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--ros2-ws)
            ros2_ws_dir="$2"
            shift # past argument
            shift # past value
            ;;
        -t|--temp-dir)
            temp_dir="$2"
            shift # past argument
            shift # past value
            ;;
        -p|--package)
            package_name="$2"
            shift # past argument
            shift # past value
            ;;
        -i|--interface)
            interface="$2"
            shift # past argument
            shift # past value
            ;;
        --internet)
            internet_required="True"
            shift # past argument
            shift # past value
            ;;
        --remove)
            remove_startup="True"
            shift # past argument
            shift # past value
            ;;
        --password)
            pswd="$2"
            shift # past argument
            shift # past value
            ;;
        -*|--*)
            echo "Unknown option $1"
            exit 0
            ;;
        *)
            shift # past argument
            ;;
    esac
done

function PrintError () # Red
{
    error_color="\033[1;91m"
    reset_color="\033[0m"
    printf "${error_color}%s${reset_color}\n" "$1"
}

function PrintSuccess () # Green
{
    success_color="\033[1;92m"
    reset_color="\033[0m"
    printf "${success_color}%s${reset_color}\n" "$1"
}

function PrintWarning () # Yellow
{
    warn_color="\033[1;93m"
    reset_color="\033[0m"
    printf "${warn_color}%s${reset_color}\n" "$1"
}

# Check Ubuntu release
ubuntu_ver=$(lsb_release -r | grep -Po '[\d.]+')
echo $ubuntu_ver
if [ "$ubuntu_ver" == "18.04" ]
then
    ros_distro="eloquent"
elif [ "$ubuntu_ver" == "20.04" ]
then
    ros_distro="foxy"
elif [ "$ubuntu_ver" == "22.04" ]
then
    ros_distro="humble"
else
    PrintError "Ubuntu release not supported. ($ubuntu_ver)"
    exit 1
fi
echo "Ubuntu release: $ubuntu_ver"
echo "Corresponding ROS2 distro: $ros_distro"

# Check input
if [ "$package_name" == "NONE" ]
then
    PrintError "--package_name need to be set."
    exit 1
fi

if [ "$interface" == "NONE" ]
then
    if [ "$remove_startup" == "False" ]
    then
        PrintError "--interface need to be set."
        exit 1
    fi
fi

if ls $ros2_ws_dir &> /dev/null
then
    echo "ROS2 workspace path: $ros2_ws_dir"
else
    PrintError "ROS2 workspace path error: $ros2_ws_dir"
    exit 1
fi

if ls $temp_dir &> /dev/null
then
    echo "Temp path: $temp_dir"
else
    if [ "$remove_startup" == "False" ]
    then
        PrintError "Temp path error: $temp_dir"
        exit 1
    fi
fi

# Check sudo
if echo $pswd | sudo -S echo "sudo check" &> /dev/null
then
    PrintSuccess "sudo checked!"
else
    PrintError "Permission denied: $pswd"
    exit 1
fi


# Files under launch directory:
# common.yaml
# launch.py
# source_env.txt
# *runfile.sh (Generated while running installation)


# Remove current run file
runfile_path="$ros2_ws_dir/src/$package_name/launch/runfile.sh"
rm -rf "$runfile_path"

# Remove current autostart file
autostart_path="/etc/xdg/autostart/rv_$package_name.desktop"
sudo rm -rf "$autostart_path"

# If remove flag sets to True, exit script
if [ "$remove_startup" == "True" ]
then
    exit 0
fi

# Install APT dependencies
if ls "$ros2_ws_dir/src/$package_name/requirements_apt.txt" &> /dev/null
then
    echo "Found APT requirements. Installing..."
    cat "$ros2_ws_dir/src/$package_name/requirements_apt.txt" | xargs sudo apt install -y
fi

# Install pip dependencies
if ls "$ros2_ws_dir/src/$package_name/requirements_pip.txt" &> /dev/null
then
    echo "Found pip requirements. Installing..."
    python3 -m pip install -r "$ros2_ws_dir/src/$package_name/requirements_pip.txt"
fi

# Generate startup file
if [ "$internet_required" == "False" ]
then
    wget "https://raw.githubusercontent.com/davidweitaiwan/RV-1.0-function-install/master/scripts/run-network-check.sh" -O "$runfile_path"
else
    wget "https://raw.githubusercontent.com/davidweitaiwan/RV-1.0-function-install/master/scripts/run-internet-check.sh" -O "$runfile_path"
fi

if cat "$ros2_ws_dir/src/$package_name/launch/source_env.txt" &> /dev/null
then
    echo "Found source file. Adding to runfile.sh..."
    cat "$ros2_ws_dir/src/$package_name/launch/source_env.txt" >> "$runfile_path"
fi

echo "source $ros2_ws_dir/install/setup.bash" >> "$runfile_path"

if [ "$ros_distro" == "eloquent" ]
then
    if ls "$ros2_ws_dir/src/$package_name/launch/launch_eloquent.py" &> /dev/null
    then
        echo "ros2 launch $package_name launch_eloquent.py" >> "$runfile_path"
    else
        PrintWarning "Cannot found relatived ROS2 distro launch file 'launch_$ros_distro.py'. Use launch.py instead may caused unknown errors."
        echo "ros2 launch $package_name launch.py" >> "$runfile_path"
    fi
else
    echo "ros2 launch $package_name launch.py" >> "$runfile_path"
fi
sudo chmod a+x $runfile_path
PrintSuccess "Run file created: $runfile_path"
# Generate autostart file
temp_path="$temp_dir/$package_name.desktop.tmp"
rm -rf $temp_path
touch $temp_path
echo "[Desktop Entry]" >> "$temp_path"
echo "Type=Application" >> "$temp_path"
echo "Exec=gnome-terminal --command '$runfile_path $interface'" >> "$temp_path"
echo "Hidden=false" >> "$temp_path"
echo "NoDisplay=false" >> "$temp_path"
echo "X-GNOME-Autostart-enabled=true" >> "$temp_path"
echo "Name[en_NG]=$package_name startup" >> "$temp_path"
echo "Name=$package_name startup" >> "$temp_path"
echo "Comment[en_NG]=Start $package_name Application" >> "$temp_path"
echo "Comment=Start $package_name Application" >> "$temp_path"
sudo cp $temp_path $autostart_path
