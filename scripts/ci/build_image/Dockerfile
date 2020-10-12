FROM ubuntu:18.04

COPY scripts scripts
RUN apt-get update \
  && apt-get install -y \
    fish sudo \
  && ./scripts/install_system_dependencies \
  && rm -rf /var/lib/apt/lists/*
RUN aqt install --outputdir /qt 5.15.1 linux desktop -m qtcharts
ENV PATH="/qt/5.15.1/gcc_64/bin:${PATH}"

