#!/usr/bin/bash
ros2_ws_dir="$HOME/ros2_ws"

while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--ros2-ws)
            ros2_ws_dir="$2"
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

if ls $ros2_ws_dir &> /dev/null
then
    echo "ROS2 workspace path: $ros2_ws_dir"
else
    echo "ROS2 workspace path error: $ros2_ws_dir"
    exit 1
fi

source /opt/ros/$ros_distro/setup.bash
cd $ros2_ws_dir
rm -rf build install log
colcon build --symlink-install
