# NVIDIA-TUNER-CPP

A simple C++ CLI tool for overclocking, undervolting and controlling the fan of NVIDIA GPUs on Linux. Using the NVML library it equally supports X11 and Wayland.

## Features

* Set core clock offset.
* Set memory clock offset.
* Set maximum boost core clock.
* Set maximum boost memory clock.
* Set power limit.
* PI-based temperature control for automatic fan management.
* Automatically set the fan control back to default on termination.

## Usage

**This tool is still under testing and it is impossible for me to guarantee that it works on every hardware, so use it at your own risk**

Show all possible options:

```bash
./nvidia-tuner --help
```

Usage example with overclocking:
```bash
./nvidia-tuner --core-clock-offset 150 --memory-clock-offset 800 --power-limit 180
```

Usage example with PI temperature control:
```bash
./nvidia-tuner --target-temperature 70 --proportional-gain 2.5 --integral-gain 0.15
```

Combined example:
```bash
./nvidia-tuner --core-clock-offset 150 --memory-clock-offset 800 --power-limit 180 --target-temperature 70
```

The PI controller automatically adjusts fan speed to maintain the target temperature. The proportional and integral gains can be tuned for different response characteristics - higher proportional gain gives faster response, while integral gain eliminates steady-state error.

## Compilation

To compile from source, you'll need the NVIDIA ML development library and CMake:

```bash
# Install dependencies
sudo apt install libnvidia-ml-dev cmake build-essential

# Clone and build
git clone https://github.com/jukofyork/nvidia-tuner-cpp
cd nvidia-tuner-cpp
mkdir build
cd build
cmake ..
make -j$(nproc)
```

The compiled binary will be located at `build/nvidia-tuner`.

## Run on startup

1. Copy the binary to `/usr/local/sbin/`.
2. Create the systemd service file `/etc/systemd/system/nvidia-tuner.service` with the following content:

```service
[Unit]
Description=NVIDIA GPU Tuner and Temperature Controller
After=graphical.target

[Service]
Type=exec
ExecStart=/usr/local/sbin/nvidia-tuner --core-clock-offset 150 --memory-clock-offset 800 --power-limit 180 --target-temperature 70
Restart=always
RestartSec=5
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=graphical.target
```

3. Reload the systemd manager configuration to recognize the new service:

```bash
sudo systemctl daemon-reload
```

4. Start the service:

```bash
sudo systemctl start nvidia-tuner.service
```

5. Enable the service to start automatically at boot:

```bash
sudo systemctl enable nvidia-tuner.service
```

6. Check the systemd journal for any errors:

```bash
sudo journalctl -u nvidia-tuner.service
```

## PI Controller Tuning

The PI controller uses two main parameters:

* **Proportional Gain (-p)**: Controls immediate response to temperature changes. Higher values = faster response but may cause oscillation.
* **Integral Gain (-i)**: Eliminates steady-state temperature error. Higher values = better accuracy but may cause instability.

Default values (4.0 and 0.2) work well for most GPUs, but you can experiment with different values if needed.