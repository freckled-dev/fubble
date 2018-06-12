
package com.freckles.of.couple.fubble.tools;

import java.io.ByteArrayOutputStream;
import java.nio.ByteBuffer;
import java.util.List;
import java.util.stream.Collectors;

import javax.websocket.Session;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.freckles.of.couple.fubble.entities.Room;
import com.freckles.of.couple.fubble.entities.User;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;

public class MessageHelper {

    private static final Logger LOGGER = LogManager.getLogger(MessageHelper.class);

    public void broadcastAsync(Room room, MessageContainerClient container) {
        List<Session> sessions = room.getUsers().stream().map(User::getSession).collect(Collectors.toList());
        broadcastAsync(sessions, container);
    }

    public void broadcastAsync(List<Session> sessions, MessageContainerClient container) {
        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            container.writeTo(output);

            sessions.forEach(session -> {
                ByteBuffer binaryMessage = ByteBuffer.wrap(output.toByteArray());
                session.getAsyncRemote().sendBinary(binaryMessage);
            });

            output.close();
        } catch (Exception ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        }
    }

    public void sendAsync(Session session, MessageContainerClient container) {
        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            container.writeTo(output);
            ByteBuffer binaryMessage = ByteBuffer.wrap(output.toByteArray());
            session.getAsyncRemote().sendBinary(binaryMessage);
            output.close();
        } catch (Exception ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        }

    }

}
