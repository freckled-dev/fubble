
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
        LOGGER.info("opening websocket");
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
        LOGGER.info("closing websocket");
        this.userSession = null;
    }

    /**
     * Callback hook for Message Events. This method will be invoked when a client send a message.
     *
     * @param message The text message
     */
    @OnMessage
    public void onMessage(String message) {
        LOGGER.info(message);
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