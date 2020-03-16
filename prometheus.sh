# prometheus
sudo useradd --no-create-home --shell /bin/false prometheus
sudo mkdir /opt/prometheus
sudo mkdir /etc/prometheus
sudo mkdir /var/lib/prometheus
sudo chown prometheus:prometheus /opt/prometheus
sudo chown prometheus:prometheus /etc/prometheus
sudo chown prometheus:prometheus /var/lib/prometheus

wget -O /tmp/prometheus-2.7.1.linux-armv7.tar.gz https://github.com/prometheus/prometheus/releases/download/v2.7.1/prometheus-2.7.1.linux-armv7.tar.gz

tar xzf /tmp/prometheus-2.7.1.linux-armv7.tar.gz -C /tmp
sudo mv /tmp/prometheus-2.7.1.linux-armv7/* /opt/prometheus
sudo chown prometheus:prometheus /opt/prometheus/*

sudo ln -s /opt/prometheus/prometheus /usr/local/bin/prometheus
sudo ln -s /opt/prometheus/promtool /usr/local/bin/promtool

sudo ln -s /opt/prometheus/console_libraries /etc/prometheus/console_libraries
sudo ln -s /opt/prometheus/consoles /etc/prometheus/consoles

sudo tee /etc/prometheus/prometheus.yml <<EOF
global:
  scrape_interval: 15s

scrape_configs:
  - job_name: 'prometheus'
    scrape_interval: 5s
    static_configs:
      - targets: ['localhost:9090']
  - job_name: 'node_exporter'
    scrape_interval: 5s
    static_configs:
      - targets: ['localhost:9100']

#remote_write:
#  - url: "http://localhost:8086/api/v1/prom/write?db=prometheus"
#
#remote_read:
#  - url: "http://localhost:8086/api/v1/prom/read?db=prometheus"
EOF

sudo chown prometheus:prometheus /etc/prometheus/*

sudo tee /etc/systemd/system/prometheus.service <<EOF
[Unit]
Description=Prometheus
Wants=network-online.target
After=network-online.target

[Service]
User=prometheus
Group=prometheus
Type=simple
ExecStart=/usr/local/bin/prometheus \
    --config.file /etc/prometheus/prometheus.yml \
    --storage.tsdb.path /var/lib/prometheus/ \
    --web.console.templates=/etc/prometheus/consoles \
    --web.console.libraries=/etc/prometheus/console_libraries
    
[Install]
WantedBy=multi-user.target
EOF

sudo systemctl daemon-reload
sudo systemctl start prometheus
sudo systemctl enable prometheus

# node_exporter
sudo useradd --no-create-home --shell /bin/false node_exporter
sudo mkdir /opt/node_exporter
sudo mkdir /var/lib/node_exporter
sudo mkdir /var/lib/node_exporter/textfile_collector
sudo chown node_exporter:node_exporter /opt/node_exporter
sudo chown node_exporter:node_exporter /var/lib/node_exporter
sudo chown node_exporter:node_exporter /var/lib/node_exporter/textfile_collector

wget -O /tmp/node_exporter-0.17.0.linux-armv7.tar.gz https://github.com/prometheus/node_exporter/releases/download/v0.17.0/node_exporter-0.17.0.linux-armv7.tar.gz

tar xzf /tmp/node_exporter-0.17.0.linux-armv7.tar.gz -C /tmp
sudo mv /tmp/node_exporter-0.17.0.linux-armv7/* /opt/node_exporter
sudo chown node_exporter:node_exporter /opt/node_exporter/*

sudo ln -s /opt/node_exporter/node_exporter /usr/local/bin/node_exporter

sudo tee /etc/systemd/system/node_exporter.service <<EOF
[Unit]
Description=Node Exporter
Wants=network-online.target
After=network-online.target

[Service]
User=node_exporter
Group=node_exporter
Type=simple
ExecStart=/usr/local/bin/node_exporter \
    --collector.textfile.directory=/var/lib/node_exporter/textfile_collector

[Install]
WantedBy=multi-user.target
EOF

sudo systemctl daemon-reload
sudo systemctl start node_exporter
sudo systemctl enable node_exporter

# raspberry_exporter
sudo tee /opt/node_exporter/raspberry_exporter <<EOF
#!/bin/bash

set -eu

VCGEN=/opt/vc/bin/vcgencmd
TEXTFILE=/var/lib/node_exporter/textfile_collector/raspberry-metrics.prom
TEMPFILE=$TEXTFILE.$$

mkdir -p /var/lib/node_exporter/textfile_collector/

cpuTemp0=$(cat /sys/class/thermal/thermal_zone0/temp)
cpuTemp1=$(($cpuTemp0/1000))
cpuTemp2=$(($cpuTemp0/100))
cpuTempM=$(($cpuTemp2 % $cpuTemp1))

CPU_TEMP="$cpuTemp1.$cpuTempM"
GPU_TEMP=$(/opt/vc/bin/vcgencmd measure_temp | grep -Eo '([0-9]+\.[0-9]+)')

if [ -z "${CPU_TEMP}" ];
then
    echo "$NOW - Error: Value variable empty"
else
    echo "CPU_Temperature ${CPU_TEMP}" > $TEMPFILE
    echo "GPU_Temperature ${GPU_TEMP}" >> $TEMPFILE

    for id in arm core h264 isp v3d uart pwm emmc pixel vec hdmi dpi; do
        echo "Freq_$id $($VCGEN measure_clock $id | awk '{split($0,a,"="); print a[2]}')" >> $TEMPFILE
    done

    for id in core sdram_c sdram_i sdram_p; do
        echo "Volt_$id $($VCGEN measure_volts $id | awk '{split($0,a,"="); print a[2]}' | sed 's/V$//')" >> $TEMPFILE
    done

    for id in arm gpu; do
        echo "Mem_$id $($VCGEN get_mem $id | awk '{split($0,a,"="); print a[2]}' | sed 's/M$//')" >> $TEMPFILE
    done

    mv $TEMPFILE $TEXTFILE
fi
EOF

sudo chown node_exporter:node_exporter /opt/node_exporter/raspberry_exporter

sudo ln -s /opt/node_exporter/raspberry_exporter /usr/local/bin/raspberry_exporter

sudo tee /etc/systemd/system/raspberry_exporter.service <<EOF
[Unit]
Description=Collect Raspberry Pi metrics for Prometheus

[Service]
Type=oneshot
ExecStart=/usr/local/bin/raspberry_exporter
EOF

sudo tee /etc/systemd/system/raspberry_exporter.timer <<EOF
[Unit]
Description=Temperature CPU Timer

[Timer]
OnCalendar=*:*:0,15,30,45

[Install]
WantedBy=timers.target
EOF

sudo systemctl daemon-reload
sudo systemctl start raspberry_exporter.timer
sudo systemctl enable raspberry_exporter.timer
