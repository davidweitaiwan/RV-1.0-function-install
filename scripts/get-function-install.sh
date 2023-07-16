#!/usr/bin/bash
# Check Internet Connection
printf "%s" "Internet connecting..."
while ! ping -w 1 -c 1 -n 168.95.1.1 &> /dev/null
do
    printf "%c" "."
done
printf "\n%s\n" "Internet connected."

# Check git
if [ -x "$(command -v git)" ]; then
    echo "Found git." && git --version
else
    echo "No git. Installing git..."
    sudo apt install git -y
fi

# Install dependencies
sudo apt install libglfw3-dev -y

ws_dir="$HOME/rv-function-install"
rm -rf $ws_dir && mkdir -p $ws_dir
wget -P $ws_dir https://github.com/davidweitaiwan/RV-1.0-function-install/raw/master/prebuild/function-install
wget -P $ws_dir https://github.com/davidweitaiwan/RV-1.0-function-install/raw/master/prebuild/imgui.ini
cd $ws_dir
sudo chmod a+x function-install
echo "Grab installer completed. Run './function-install' to start installation."
