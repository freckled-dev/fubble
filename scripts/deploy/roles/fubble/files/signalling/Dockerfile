FROM opensuse/tumbleweed:latest

RUN zypper install -y gcc-c++ libasan4
RUN mkdir /fubble
WORKDIR /fubble
COPY install ./
ENV LD_LIBRARY_PATH="/fubble/lib64:/fubble/lib/x86_64-linux-gnu"
ENTRYPOINT ["./bin/signalling_server", "-p", "8000"]
EXPOSE 8000

