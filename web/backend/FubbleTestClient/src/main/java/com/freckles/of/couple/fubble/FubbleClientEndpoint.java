
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

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.freckles.of.couple.fubble.entities.User;
import com.freckles.of.couple.fubble.proto.WebContainer.ChatMessageClient;
import com.freckles.of.couple.fubble.proto.WebContainer.JoinedRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.LockedRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient.MessageTypeCase;
import com.freckles.of.couple.fubble.proto.WebContainer.RenamedUser;
import com.freckles.of.couple.fubble.proto.WebContainer.UserJoined;
import com.freckles.of.couple.fubble.proto.WebContainer.UserLeft;
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
    private List<User>                   users       = new ArrayList<User>();
    private boolean                      printSysout = false;

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

            if (MessageTypeCase.JOINED_ROOM.equals(messageTypeCase)) {
                JoinedRoom joinedRoom = container.getJoinedRoom();
                this.userId = joinedRoom.getUserId();
                this.userName = joinedRoom.getUserName();

                print("-----------------------------------");
                print(joinedRoom.getUserName());
                print("-----------------------------------");
            }

            if (MessageTypeCase.USER_JOINED.equals(messageTypeCase)) {
                UserJoined userJoined = container.getUserJoined();

                users.add(new User(userJoined.getUserId(), userJoined.getUserName()));

                if (!userJoined.getUserId().equals(userId)) {
                    print(String.format("A new user joined the room - %s.", userJoined.getUserName()));
                }
            }

            if (MessageTypeCase.RENAMED_USER.equals(messageTypeCase)) {
                RenamedUser renamedUser = container.getRenamedUser();

                if (renamedUser.getUserId().equals(userId)) {
                    this.userName = renamedUser.getNewName();
                }

                User updateUser = users.stream().filter(user -> user.getId().equals(renamedUser.getUserId())).findFirst().get();
                updateUser.setName(renamedUser.getNewName());
                print(String.format("User %s changed his name to %s.", renamedUser.getUserId(), renamedUser.getNewName()));
            }

            if (MessageTypeCase.CHAT_MESSAGE.equals(messageTypeCase)) {
                ChatMessageClient chat = container.getChatMessage();
                User chatUser = users.stream().filter(user -> user.getId().equals(chat.getUserId())).findFirst().get();

                print(String.format("%s: %s", chatUser.getName(), chat.getContent()));
            }

            if (MessageTypeCase.USER_LEFT.equals(messageTypeCase)) {
                UserLeft userLeft = container.getUserLeft();

                User left = users.stream().filter(user -> user.getId().equals(userLeft.getUserId())).findFirst().get();
                print(String.format("User %s has left the building!", left.getName()));
            }

            if (MessageTypeCase.LOCK_ROOM.equals(messageTypeCase)) {
                LockedRoom userLeft = container.getLockRoom();
                System.out.println(userLeft);
            }

            messages.add(container);
        } catch (InvalidProtocolBufferException ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        }

    }

    private void print(String sysOut) {
        if (printSysout) {
            System.out.println(sysOut);
        }
    }

    public String getUserName() {
        return userName;
    }

    public void setUserName(String userName) {
        this.userName = userName;
    }

    public void setPrintSysout(boolean printSysout) {
        this.printSysout = printSysout;
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
        } catch (Exception ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
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