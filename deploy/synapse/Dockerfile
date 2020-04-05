FROM matrixdotorg/synapse:latest

# TODO move to /data which is a volume. got permission problems so far

RUN mkdir /fubble
WORKDIR /fubble
COPY ./fubble.io.log.config ./fubble.io.signing.key ./homeserver.yaml ./
ENV SYNAPSE_CONFIG_PATH=/fubble
RUN chmod -R a+rwx /fubble

