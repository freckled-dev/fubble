FROM matrixdotorg/synapse:v1.18.0

# they switched from alpine to buster, which already includes bash
# RUN apk add --no-cache bash
WORKDIR /data
COPY homeserver.yaml install_homeserver /fubble/
ENTRYPOINT ["/fubble/install_homeserver"]

