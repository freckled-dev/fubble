
package com.freckles.of.couple.fubble;

import java.net.URI;
import java.nio.ByteBuffer;

import javax.websocket.ClientEndpoint;
import javax.websocket.CloseReason;
import javax.websocket.ContainerProvider;
import javax.websocket.OnClose;
import javax.websocket.OnMessage;
import javax.websocket.OnOpen;
import javax.websocket.Session;
import javax.websocket.WebSocketContainer;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient.MessageTypeCase;
import com.google.protobuf.InvalidProtocolBufferException;

/**
 * ChatServer Client
 */
@ClientEndpoint
public class FubbleClientEndpoint {

    private static final Logger LOGGER      = LogManager.getLogger(FubbleClientEndpoint.class);

    private Session             userSession = null;

    public FubbleClientEndpoint(URI endpointURI) {
        try {
            WebSocketContainer container = ContainerProvider.getWebSocketContainer();
            container.connectToServer(this, endpointURI);
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    /**
     * Callback hook for Connection open events.
     *
     * @param userSession the userSession which is opened.
     */
    @OnOpen
    public void onOpen(Session userSession) {
        this.userSession = userSession;
    }

    /**
     * Callback hook for Connection close events.
     *
     * @param userSession the userSession which is getting closed.
     * @param reason the reason for connection close
     */
    @OnClose
    public void onClose(Session userSession, CloseReason reason) {
        this.userSession = null;
    }

    /**
     * Callback hook for Message Events. This method will be invoked when a client send a message.
     *
     * @param message The text message
     */
    @OnMessage
    public void onMessage(byte[] message) {
        ByteBuffer binaryMessage = ByteBuffer.wrap(message);
        try {
            MessageContainerClient container = MessageContainerClient.parseFrom(binaryMessage);
            MessageTypeCase messageTypeCase = container.getMessageTypeCase();

            if (MessageTypeCase.JOINEDROOM.equals(messageTypeCase)) {
                System.out.println("JoinedRoom");
                System.out.println(container);
            }
        } catch (InvalidProtocolBufferException ex) {
            LOGGER.error(ex);
        }

    }

    /**
     * Send a message.
     *
     * @param message
     */
    public void sendMessage(byte[] message) {
        ByteBuffer binaryMessage = ByteBuffer.wrap(message);
        try {
            this.userSession.getAsyncRemote().sendBinary(binaryMessage);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

}