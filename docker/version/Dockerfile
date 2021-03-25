FROM registry.gitlab.com/acof/fubble/server:latest

RUN mkdir /fubble
WORKDIR /fubble
COPY . ./
ENV LD_LIBRARY_PATH="/fubble/lib64:/fubble/lib/x86_64-linux-gnu"
ENTRYPOINT ["./bin/version_server"]
EXPOSE 8085


