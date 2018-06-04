
package com.freckles.of.couple.fubble.tools;

import java.nio.ByteBuffer;
import java.util.List;

import javax.websocket.Session;

public class MessageHelper {

    public void broadcastAsync(List<Session> sessions, byte[] message) {
        sessions.forEach(session -> {
            ByteBuffer binaryMessage = ByteBuffer.wrap(message);
            session.getAsyncRemote().sendBinary(binaryMessage);
        });
    }

    public void sendAsync(Session session, byte[] message) {
        ByteBuffer binaryMessage = ByteBuffer.wrap(message);
        session.getAsyncRemote().sendBinary(binaryMessage);
    }

}
