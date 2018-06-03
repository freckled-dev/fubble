
package com.freckles.of.couple.fubble;

import java.io.IOException;
import java.nio.ByteBuffer;

import javax.websocket.OnClose;
import javax.websocket.OnError;
import javax.websocket.OnMessage;
import javax.websocket.OnOpen;
import javax.websocket.Session;
import javax.websocket.server.ServerEndpoint;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.freckles.of.couple.fubble.handler.FubbleMessageHandler;
import com.freckles.of.couple.fubble.handler.JoinRoomHandler;
import com.freckles.of.couple.fubble.handler.RenameRoomHandler;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainer;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainer.MessageTypeCase;

@ServerEndpoint(value = "/")
public class FubbleEndpoint {

    private static final Logger  LOGGER = LogManager.getLogger(FubbleEndpoint.class);
    private FubbleMessageHandler handler;

    @OnOpen
    public void onOpen(Session session) {
        LOGGER.info("Server: A new client has connected.");
    }

    @OnMessage
    public void onMessage(Session session, byte[] message)
        throws IOException {
        LOGGER.info("Server: Message received.");

        ByteBuffer binaryMessage = ByteBuffer.wrap(message);
        MessageContainer container = MessageContainer.parseFrom(binaryMessage);

        handleContainer(container, session);

    }

    private void handleContainer(MessageContainer container, Session session) {
        MessageTypeCase messageTypeCase = container.getMessageTypeCase();

        if (messageTypeCase.equals(MessageTypeCase.JOINROOM)) {
            handler = new JoinRoomHandler();
        }

        if (messageTypeCase.equals(MessageTypeCase.RENAMEROOM)) {
            handler = new RenameRoomHandler();
        }

        handler.handleMessage(container, session);
    }

    @OnError
    public void onError(Session session, Throwable throwable) {}

    @OnClose
    public void onClose(Session session)
        throws IOException {}

}
