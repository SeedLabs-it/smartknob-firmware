#!/bin/bash
export PATH=$PATH:$HOME/.platformio/penv/bin

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
if ! command -v platformio &> /dev/null 
then
    echo "Platformio is not installed."
    read -p "Do you want to install PlatformIO via curl? (y/n): " choice
    case "$choice" in 
      y|Y ) echo "Installing PlatformIO..."
            curl -fsSL -o get-platformio.py https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py
            python3 get-platformio.py
            echo "PlatformIO installed successfully.";;
      n|N ) echo "PlatformIO installation required" 
            exit 1;;
    * )     echo "Invalid input. Please press y or n."
            exit 1;;
    esac
else
    echo "Platformio is already installed."
fi

echo ""

# Check if esp-idf is installed
if [ ! -f ./software/release/esp-idf/export.sh ]; then
    echo "ESP-IDF is not installed."
    read -p "Do you want to install ESP-IDF? (y/n): " choice
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

# Source the export.sh file
. ./software/release/esp-idf/export.sh >/dev/null 2>&1


#Get Platformio device port for esp32
device=$(platformio device list | grep -B 2 "VID:PID=303A:1001" | grep -o -E '/dev/(tty|cu)\S*')

if [ -z "$device" ]; then
    echo "Device not found. Please connect the device and try again."
    exit 1
fi

echo "Device port: $device"

echo "FW version used $latest_tag"
echo "Recovery FW version used $latest_recovery_tag"
read -p "Enter order id: " order

echo ""

start=$(date +%s)
mac_address=$(platformio device list | grep -B2 'VID:PID=303A:1001' | grep 'SER=' | grep -o 'SER=[^ ]*' | grep -o '[^=]*$')
echo "[0/4] Saving mac address $mac_address"
echo "$order - $mac_address" >> "./software/release/mac_addresses.txt"
end=$(date +%s)
echo "[0/4] done in $(($end-$start)) seconds"

start=$(date +%s)
echo "[1/4] Erasing flash..."
~/.platformio/packages/tool-esptoolpy/esptool.py --chip esp32s3 --port $device --baud 921600 --before default_reset --after hard_reset erase_flash > /dev/null
end=$(date +%s)
echo "[1/4] done in $(($end-$start)) seconds"

start=$(date +%s)
echo "[2/4] Uploading recovery firmware..."
#find way to have same partitions for recovery and firmware etc
~/.platformio/packages/tool-esptoolpy/esptool.py --chip esp32s3 --port $device --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 16MB \
0x0000 $dist_recovery_path/bootloader.bin \
0x8000 $dist_recovery_path/partitions.bin \
0xe000 $dist_recovery_path/boot_app0.bin \
0x10000 $dist_recovery_path/firmware.bin > /dev/null
end=$(date +%s)
echo "[2/4] done in $(($end-$start)) seconds"

start=$(date +%s)
echo "[3/4] Uploading firmware..."
~/.platformio/packages/tool-esptoolpy/esptool.py --chip esp32s3 --port $device --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 16MB \
0x610000 $dist_path/firmware.bin > /dev/null
otatool.py -p $device switch_ota_partition --slot 1 > /dev/null 
end=$(date +%s)
echo "[3/4] done in $(($end-$start)) seconds"

start=$(date +%s)
echo "[3/4] Connect to serial..."
(while ! test -c $device; do sleep 1; echo 'connecting...'; done; )
platformio device monitor --baud 115200

deactivate
end=$(date +%s)
echo "[4/4] done in $(($end-$start)) seconds"

echo "Finished in $(($end-$start_0)) seconds"

