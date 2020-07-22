FROM ubuntu:18.04

COPY scripts scripts
RUN apt-get update \
  && apt-get install -y \
    fish sudo \
  && ./scripts/install_system_dependencies \
  && apt purge -y \
    qt5-default qttools5-dev-tools qtquickcontrols2-5-dev qtmultimedia5-dev \
    qtdeclarative5-dev libqt5charts5-dev libqt5svg5-dev \
    qml-module-qtmultimedia qml-module-qtcharts \
  && apt autoremove -y \
  && apt install -y libgl1 libpulse-mainloop-glib0 \
  && rm -rf /var/lib/apt/lists/*
RUN aqt install --outputdir /qt 5.12.9 linux desktop -m qtcharts
ENV PATH="/qt/5.12.9/gcc_64/bin:${PATH}"

