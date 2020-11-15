FROM registry.gitlab.com/acof/fubble-deploy/server:latest

RUN mkdir /fubble
WORKDIR /fubble
COPY . ./
ENV LD_LIBRARY_PATH="/fubble/lib64:/fubble/lib/x86_64-linux-gnu"
ENTRYPOINT ["./bin/signaling_server", "-p", "8000"]
EXPOSE 8000

