FROM registry.gitlab.com/acof/fubble/server:latest

RUN mkdir /fubble
WORKDIR /fubble
COPY . ./
ENV LD_LIBRARY_PATH="/fubble/lib64:/fubble/lib/x86_64-linux-gnu"
ENTRYPOINT "./bin/temporary_room_server" "-p" "8001" "--matrix-user" $FUBBLE_MATRIX_USER "--matrix-password" $FUBBLE_MATRIX_PASSWORD "--matrix-device-id" $FUBBLE_MATRIX_DEVICE_ID "--matrix-server" "synapse"
EXPOSE 8001

