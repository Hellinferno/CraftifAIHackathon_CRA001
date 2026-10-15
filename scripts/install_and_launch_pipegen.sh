#!/usr/bin/env bash
set -euo pipefail

echo "=== SafeZone-RT: PipeGen v0.5.1 Automated Setup & Launcher ==="
if [ -f ./pipegen_prerequisites_ubuntu_x86.sh ]; then
    chmod +x ./pipegen_prerequisites_ubuntu_x86.sh
    sudo ./pipegen_prerequisites_ubuntu_x86.sh -y
fi

if [ -f ./PipeGen_0.5.1_amd64.deb ]; then
    sudo dpkg -i ./PipeGen_0.5.1_amd64.deb
fi

sudo chown -R "$USER:$(id -gn)" /opt/PipeGen
mkdir -p ~/.pipegen "/home/$USER/videos"

echo "Verifying PipeGen v0.5.1..."
pipegen --version
echo "Launching PipeGen v0.5.1..."
exec sg docker -c "pipegen"
