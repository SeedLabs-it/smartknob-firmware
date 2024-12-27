#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color



git fetch --tags

latest_tag=$(git describe --tags `git rev-list --tags --max-count=1`)
latest_recovery_tag=$(git ls-remote --tags "https://github.com/SeedLabs-it/skdk-recovery-firmware/" | awk '{print $2}' | sed 's|refs/tags/||' | sort -V | tail -n 1)

dist_path="./software/release/dist/$latest_tag/"
dist_recovery_path="./software/release/dist/recovery/$latest_recovery_tag/"

#Check if latest release on git is downloaded
if [ ! -d "$dist_path" ] || [ ! -d "$dist_recovery_path" ]; then
    echo "Latest release not downloaded."
    read -p "Would you like to download the latest release? (y/n): " response
    
    case $response in
        [Yy]* ) 
            echo "Downloading latest release..."
            ./software/release/download_latest.sh
            ./software/release/download_latest_recovery.sh
            echo ""
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

# Check if Platformio is installed
# if ! command -v platformio &> /dev/null 
# then
#     echo "Platformio is not installed."
#     read -p "Do you want to install PlatformIO via curl? (y/n): " choice
#     case "$choice" in 
#       y|Y ) echo "Installing PlatformIO..."
#             curl -fsSL -o get-platformio.py https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py
#             python3 get-platformio.py
#             echo "PlatformIO installed successfully.";;
#       n|N ) echo "PlatformIO installation required" 
#             exit 1;;
#     * )     echo "Invalid input. Please press y or n."
#             exit 1;;
#     esac
# else
#     echo "Platformio is already installed."
# fi

echo ""

# Check if esp-idf is installed
if [ ! -f ./software/release/esp-idf/export.sh ]; then
    echo "ESP-IDF is not cloned/installed."
    read -p "Do you want to clone ESP-IDF? (y/n): " choice
    case "$choice" in 
      y|Y )     echo "Cloning ESP-IDF..."
                git clone --recursive https://github.com/espressif/esp-idf ./software/release/esp-idf
                echo "ESP-IDF cloned successfully.";;
        n|N )   echo "ESP-IDF repository is required to proceed."
                exit 1;;
        * )     echo "Invalid input. Please press y or n."
                exit 1;;
    esac
else
    echo "ESP-IDF already installed."
fi

echo ""

# Check if idf.py is available
if ! command -v idf.py &> /dev/null; then
    echo "Initializing ESP-IDF..."
    ./software/release/esp-idf/install.sh > /dev/null
    . ./software/release/esp-idf/export.sh > /dev/null
        
else
    echo "ESP-IDF is already cloned&installed, idf.py is available."
fi

echo ""

#Get Platformio device port for esp32
device=$(platformio device list | grep -B 2 "VID:PID=303A:1001" | grep -o -E '/dev/(tty|cu)\S*')

if [ -z "$device" ]; then
    echo "Device not found. Please connect the device and try again."
    exit 1
fi

echo "Device port: $device"

echo -e "FW version used ${GREEN}$latest_tag${NC}"
echo -e "Recovery FW version used ${GREEN}$latest_recovery_tag${NC}"
read -p "$(echo -e "${RED}Enter order id: ${NC}")" order

echo ""

start=$(date +%s)
mac_address=$(platformio device list | grep -B2 'VID:PID=303A:1001' | grep 'SER=' | grep -o 'SER=[^ ]*' | grep -o '[^=]*$')
echo "[0/6] Saving mac address $mac_address"
echo "$order - $mac_address" >> "./software/release/mac_addresses.txt"
end=$(date +%s)
echo -e "[0/6] Done in $(($end-$start)) seconds \n"

start=$(date +%s)
echo "[1/6] Erasing flash..."
~/.platformio/packages/tool-esptoolpy/esptool.py --chip esp32s3 --port $device --baud 921600 --before default_reset --after no_reset erase_flash > /dev/null
end=$(date +%s)
echo -e "[1/6] Done in $(($end-$start)) seconds\n"

start=$(date +%s)
echo "[2/6] Uploading recovery firmware..."
#find way to have same partitions for recovery and firmware etc
~/.platformio/packages/tool-esptoolpy/esptool.py --chip esp32s3 --port $device --baud 921600 --before default_reset --after no_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 16MB \
0x0000 $dist_recovery_path/bootloader.bin \
0x8000 $dist_recovery_path/partitions.bin \
0xe000 $dist_recovery_path/boot_app0.bin \
0x10000 $dist_recovery_path/firmware.bin > /dev/null
end=$(date +%s)
echo -e "[2/6] Done in $(($end-$start)) seconds\n"

start=$(date +%s)
echo "[3/6] Uploading firmware..."
~/.platformio/packages/tool-esptoolpy/esptool.py --chip esp32s3 --port $device --baud 921600 --before default_reset --after no_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 16MB \
0x610000 $dist_path/firmware.bin \
0xC50000 $dist_path/fatfs.bin
end=$(date +%s)
echo -e "[3/6] Done in $(($end-$start)) seconds\n"

# start=$(date +%s)
# echo "[4/6] Uploading filesystem..."
# ~/.platformio/packages/tool-esptoolpy/esptool.py --chip esp32s3 --port $device --baud 921600 --before default_reset --after no_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 16MB \
# 0xC50000 $dist_path/fatfs.bin
# end=$(date +%s)
# echo -e "[4/6] Done in $(($end-$start)) seconds\n"

start=$(date +%s)
echo "[5/6] Switching to release firmware OTA partition..."
otatool.py -p $device switch_ota_partition --slot 1
end=$(date +%s)
echo -e "[5/6] Done in $(($end-$start)) seconds\n"


start=$(date +%s)
echo "[6/6] Connect to serial..."
(while ! test -c $device; do sleep 1; echo 'connecting...'; done; )
platformio device monitor --baud 115200
end=$(date +%s)
echo -e "[6/6] Done in $(($end-$start)) seconds\n"
echo "Finished in $(($end-$start_0)) seconds"

deactivate