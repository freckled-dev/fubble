FROM opensuse/tumbleweed:latest

RUN zypper install -y gcc-c++ bash libasan4
RUN mkdir /fubble
WORKDIR /fubble
COPY wait-for-it.sh install ./
ENV LD_LIBRARY_PATH=/fubble/lib64
ENTRYPOINT ["./wait-for-it.sh", "--strict", "synapse:8008", "--", \
  "./bin/temporary_room_server", "-p", "8001", \
  "--matrix-server", "synapse"]
EXPOSE 8001

