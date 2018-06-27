FROM ubuntu:18.04
MAINTAINER Manuela Lanner, Markus Lanner
RUN apt-get update \
  && apt-get install -y \
    python3-pip \
  && pip3 install --upgrade pip
COPY ./server-1.0.0-py3-none-any.whl /
RUN pip3 install /server-1.0.0-py3-none-any.whl \
  && pip3 list
ENTRYPOINT python3 -m server
#ENTRYPOINT pip3 list

