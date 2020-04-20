FROM ubuntu:18.04

COPY scripts scripts
RUN apt-get update \
  && apt-get install -y \
    fish sudo docker-compose \
  && ./scripts/install_system_dependencies \
  && rm -rf /var/lib/apt/lists/*

