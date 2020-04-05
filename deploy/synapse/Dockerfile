FROM matrixdotorg/synapse:latest

# TODO move to /data which is a volume. got permission problems so far

RUN mkdir /fubble
WORKDIR /fubble
RUN python -m synapse.app.homeserver -c homeserver.yaml --generate-config \
  --server-name=fubble.io --report-stats=no
COPY ./homeserver.yaml ./
ENV SYNAPSE_CONFIG_PATH=/fubble
RUN chmod -R a+rwx /fubble

