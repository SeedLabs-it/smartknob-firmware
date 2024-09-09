python3 ~/.platformio/packages/tool-esptoolpy/esptool.py --chip esp32s3 --port /dev/ttyACM1 --baud 921600 write_flash --flash_mode dio --flash_freq 40m --flash_size 16MB \
0x1000 ./software/release/dist/0.4.0-rc1/bootloader.bin \
0x8000 ./software/release/dist/0.4.0-rc1/partitions.bin \
0xE000 ./software/release/dist/0.4.0-rc1/boot_app0.bin \
0x10000 ./software/release/dist/0.4.0-rc1/firmware.bin



python3 ~/.platformio/packages/tool-esptoolpy/esptool.py --chip esp32s3 --port "/dev/ttyACM1" --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 16MB \
0x0000 ./software/release/dist/0.4.0-rc1/bootloader.bin \
0x8000 ./software/release/dist/0.4.0-rc1/partitions.bin \
0xe000 ./software/release/dist/0.4.0-rc1/boot_app0.bin \
0x10000 ./software/release/dist/0.4.0-rc1/firmware.bin