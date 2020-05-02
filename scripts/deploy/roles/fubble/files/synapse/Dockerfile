FROM matrixdotorg/synapse:latest

RUN apk add --no-cache bash
WORKDIR /data
COPY homeserver.yaml install_homeserver /fubble/
ENTRYPOINT ["/fubble/install_homeserver"]

