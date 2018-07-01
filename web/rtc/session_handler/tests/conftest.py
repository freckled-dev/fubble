import os
import tempfile
import pytest

import grpc
from server import create_app
from server.server_grpc import start_server
import server.web_rtc_pb2_grpc

@pytest.fixture
def app():
    db_fd, db_path = tempfile.mkstemp()
    app = create_app({
        'TESTING': True,
        'DATABASE': db_path,
    })

    yield app

    os.close(db_fd)
    os.unlink(db_path)

@pytest.fixture
def client(app):
    return app.test_client()

@pytest.fixture
def runner(app):
    return app.test_cli_runner()

@pytest.fixture
def rtc_server():
    server_ = start_server()

    yield server_

    server_.stop(0)

@pytest.fixture
def rtc_stub(rtc_server):
    channel = grpc.insecure_channel('localhost:20051')
    stub = server.web_rtc_pb2_grpc.RtcStub(channel)

    yield stub

    channel.close()
