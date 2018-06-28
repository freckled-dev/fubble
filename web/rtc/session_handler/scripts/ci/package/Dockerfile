FROM registry.gitlab.com/acof/fubble/ci/session_handler_base:v0
COPY ./server-1.0.0-py3-none-any.whl /
RUN pip3 install /server-1.0.0-py3-none-any.whl \
  && pip3 list
ENTRYPOINT python3 -m server

