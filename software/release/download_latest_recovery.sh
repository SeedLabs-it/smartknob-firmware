#!/bin/sh

latest_tag=$(git ls-remote --tags "https://github.com/SeedLabs-it/skdk-recovery-firmware/" | awk '{print $2}' | sed 's|refs/tags/||' | sort -V | tail -n 1)
base_url="https://github.com/SeedLabs-it/skdk-recovery-firmware/releases/download/$latest_tag"
download_path="./software/release/temp/recovery"
extract_path="./software/release/dist/recovery/$latest_tag/"

mkdir -p "$download_path"
mkdir -p "$extract_path"

echo "Downloading latest release files for tag: $latest_tag"

file="$latest_tag.zip"

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