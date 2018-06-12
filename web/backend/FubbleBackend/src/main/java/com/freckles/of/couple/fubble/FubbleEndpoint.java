
package com.freckles.of.couple.fubble;

import java.io.EOFException;
import java.io.IOException;
import java.nio.ByteBuffer;

import javax.websocket.OnClose;
import javax.websocket.OnError;
import javax.websocket.OnMessage;
import javax.websocket.OnOpen;
import javax.websocket.Session;
import javax.websocket.server.ServerEndpoint;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.freckles.of.couple.fubble.entities.Room;
import com.freckles.of.couple.fubble.entities.User;
import com.freckles.of.couple.fubble.handler.ChatMessageHandler;
import com.freckles.of.couple.fubble.handler.CloseHandler;
import com.freckles.of.couple.fubble.handler.FubbleMessageHandler;
import com.freckles.of.couple.fubble.handler.JoinRoomHandler;
import com.freckles.of.couple.fubble.handler.LockRoomHandler;
import com.freckles.of.couple.fubble.handler.RenameUserHandler;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer.MessageTypeCase;

@ServerEndpoint(value = "/")
public class FubbleEndpoint {

    private static final Logger  LOGGER = LogManager.getLogger(FubbleEndpoint.class);

    private FubbleMessageHandler handler;
    private CloseHandler         closer = new CloseHandler();
    private Session              session;
    private Room                 room;
    private User                 user;

    @OnOpen
    public void onOpen(Session session) {
        this.session = session;
    }

    @OnMessage
    public void onMessage(byte[] message)
        throws IOException {
        ByteBuffer binaryMessage = ByteBuffer.wrap(message);
        MessageContainerServer container = MessageContainerServer.parseFrom(binaryMessage);

        try {
            if (room != null) {
                room.lock();
            }

            handleContainer(container);
        } catch (Exception ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        } finally {
            if (room != null) {
                room.unlock();
            }
        }
    }

    private void handleContainer(MessageContainerServer container) {
        MessageTypeCase messageTypeCase = container.getMessageTypeCase();

        if (messageTypeCase.equals(MessageTypeCase.JOIN_ROOM)) {
            handler = new JoinRoomHandler();
        }

        if (messageTypeCase.equals(MessageTypeCase.CHAT_MESSAGE)) {
            handler = new ChatMessageHandler();
        }

        if (messageTypeCase.equals(MessageTypeCase.RENAME_USER)) {
            handler = new RenameUserHandler();
        }

        if (messageTypeCase.equals(MessageTypeCase.LOCK_ROOM)) {
            handler = new LockRoomHandler();
        }

        handler.handleMessage(container, this);
    }

    public void setRoom(Room room) {
        if (room == null) {
            return;
        }

        if (this.room == null) {
            this.room = room;
            this.room.lock();
        } else {
            if (!this.room.equals(room)) {
                LOGGER.error("TODO"); // böser hacker
            }
        }
    }

    public Session getSession() {
        return this.session;
    }

    public Room getRoom() {
        return this.room;
    }

    public User getUser() {
        return this.user;
    }

    public void setUser(User user) {
        this.user = user;
    }

    @OnError
    public void onError(Throwable throwable) {
        // EOF = user closed their browser, etc
        if (throwable instanceof EOFException) {
            return;
        } else {
            LOGGER.error(ExceptionUtils.getStackTrace(throwable));
        }
    }

    @OnClose
    public void onClose()
        throws IOException {
        handleUserQuit();
    }

    private void handleUserQuit() {
        if (room == null) {
            return;
        }

        room.lock();

        try {
            closer.handleClose(this);
        } catch (Exception ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        } finally {
            room.unlock();
        }
    }

}
