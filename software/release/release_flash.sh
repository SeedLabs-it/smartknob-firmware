#!/bin/bash
export PATH=$PATH:$HOME/.platformio/penv/bin

#Get pio device port for esp32
device=$(pio device list | grep -B 2 "VID:PID=303A:1001" | grep -o -E '/dev/(tty|cu)\w+')

echo "Device port: $device"

latest_tag=$(git describe --tags `git rev-list --tags --max-count=1`)
dist_path="./software/release/dist/$latest_tag/"

#Check if latest release on git is downloaded
if [ ! -d "$dist_path" ]; then
    echo "Latest release not downloaded."
    read -p "Would you like to download the latest release? (y/n): " response
    
    case $response in
        [Yy]* ) 
            echo "Downloading latest release..."
            ./software/release/download_latest.sh
            ;;
        [Nn]* ) 
            echo "Download aborted. Exiting."
            exit 1
            ;;
        * ) 
            echo "Invalid response. Please answer y or n."
            exit 1
            ;;
    esac
else
    echo "Latest release already downloaded."
fi

# Check if pio is installed
if ! command -v pio &> /dev/null
then
    echo "pio is not installed."
    read -p "Do you want to install PlatformIO via curl? (y/n): " choice
    case "$choice" in 
      y|Y ) echo "Installing PlatformIO..."
            curl -fsSL -o get-platformio.py https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py
            python3 get-platformio.py
            echo "PlatformIO installed successfully.";;
      n|N ) echo "Skipping PlatformIO installation.";;
      * ) echo "Invalid input. Please press y or n.";;
    esac
else
    echo "pio is already installed."
fi

read -p "Enter order id: " order

start=$(date +%s)
mac_address=$(python3 ./software/release/get_mac_address.py)
echo "[0/4] Saving mac address $mac_address"
echo "$order - $mac_address" >> "./software/release/mac_addresses.txt"
end=$(date +%s)
echo "[0/4] done in $(($end-$start)) seconds"

start=$(date +%s)
echo "[1/4] Erasing flash..."
platformio run -t erase > /dev/null
end=$(date +%s)
echo "[1/4] done in $(($end-$start)) seconds"

# start=$(date +%s)
# echo "[2/4] Flashing filesystem..."
# platformio run -t upload > /dev/null
# end=$(date +%s)
# echo "[2/4] done in $(($end-$start)) seconds"

start=$(date +%s)
echo "[3/4] Uploading firmware..."
~/.platformio/packages/tool-esptoolpy/esptool.py --chip esp32s3 --port "/dev/ttyACM1" --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 16MB \
0x0000 ./software/release/dist/0.4.0-rc1/bootloader.bin \
0x8000 ./software/release/dist/0.4.0-rc1/partitions.bin \
0xe000 ./software/release/dist/0.4.0-rc1/boot_app0.bin \
0x10000 ./software/release/dist/0.4.0-rc1/firmware.bin > /dev/null
# ~/.platformio/packages/tool-esptoolpy/esptool.py --baud 921600 write_flash -z 0x00010000 $device 0x1000 bootloader.bin \
end=$(date +%s)
echo "[3/4] done in $(($end-$start)) seconds"

start=$(date +%s)
echo "[4/4] Connect to serial..."
(while ! test -c $device; do sleep 1; echo 'connecting...'; done; )
platformio device monitor --baud 115200

deactivate
end=$(date +%s)
echo "[4/4] done in $(($end-$start)) seconds"

echo "Finished in $(($end-$start_0)) seconds"

