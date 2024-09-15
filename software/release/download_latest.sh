#!/bin/sh

latest_tag=$(git describe --tags `git rev-list --tags --max-count=1`)
base_url="https://github.com/SeedLabs-it/smartknob-firmware/releases/download/$latest_tag"
download_path="./software/release/temp"
extract_path="./software/release/dist/$latest_tag/"

mkdir -p "$download_path"
mkdir -p "$extract_path"

echo "Downloading latest release files for tag: $latest_tag"

file="0.4.0-rc1.zip"

file_url="$base_url/$latest_tag.zip"
echo "Downloading $file_url..."

curl -L -o "$download_path/$file" "$file_url"
unzip -o "$download_path/$file" -d "$extract_path"

# Check if the download was successful
if [ $? -eq 0 ]; then
    echo "$file downloaded successfully."
else
    echo "Error downloading $file. Exiting."
    exit 1
fi

echo "All files downloaded to $download_path."
echo "Cleanup"

rm -rf "$download_path"

echo "Download and extraction complete."