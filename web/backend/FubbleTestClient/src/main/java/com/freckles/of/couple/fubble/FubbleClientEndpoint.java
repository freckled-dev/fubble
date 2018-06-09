
package com.freckles.of.couple.fubble;

import java.net.URI;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

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

import com.freckles.of.couple.fubble.proto.WebContainer.JoinedRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient.MessageTypeCase;
import com.freckles.of.couple.fubble.proto.WebContainer.RenamedUser;
import com.freckles.of.couple.fubble.proto.WebContainer.UserJoined;
import com.google.protobuf.InvalidProtocolBufferException;

/**
 * ChatServer Client
 */
@ClientEndpoint
public class FubbleClientEndpoint {

    private static final Logger          LOGGER      = LogManager.getLogger(FubbleClientEndpoint.class);

    private Session                      userSession = null;
    private String                       userName;
    private String                       userId;

    private List<MessageContainerClient> messages    = new ArrayList<>();

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
                JoinedRoom joinedRoom = container.getJoinedRoom();
                this.userId = joinedRoom.getUserId();
            }

            if (MessageTypeCase.USERJOINED.equals(messageTypeCase)) {
                UserJoined userJoined = container.getUserJoined();

                if (userJoined.getId().equals(userId)) {
                    this.userName = userJoined.getName();
                }
            }

            if (MessageTypeCase.RENAMEDUSER.equals(messageTypeCase)) {
                RenamedUser renamedUser = container.getRenamedUser();
                System.out.println(container);

                if (renamedUser.getId().equals(userId)) {
                    this.userName = renamedUser.getName();
                }
            }

            messages.add(container);
        } catch (InvalidProtocolBufferException ex) {
            LOGGER.error(ex);
        }

    }

    public String getUserName() {
        return userName;
    }

    public void setUserName(String userName) {
        this.userName = userName;
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

    public Session getUserSession() {
        return userSession;
    }

    public List<MessageContainerClient> getMessages() {
        return messages;
    }

    public void setMessages(List<MessageContainerClient> messages) {
        this.messages = messages;
    }

    public String getUserId() {
        return userId;
    }

}