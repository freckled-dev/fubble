
package com.freckles.of.couple.fubble.tools;

import javax.websocket.Session;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.freckles.of.couple.fubble.entities.Chat;
import com.freckles.of.couple.fubble.entities.MessageContainer;
import com.google.gson.Gson;

public class MessageHandler {

    private static final Logger LOGGER  = LogManager.getLogger(MessageHandler.class);
    private BroadcastHandler    handler = new BroadcastHandler();
    private Gson                gson    = new Gson();

    public void decode(Session session, MessageContainer message) {

        switch (message.getType()) {
            case CHAT:
                handleChatMessage(session, message);
                break;

            case RENAME_USER:
                handleRenameMessage(session, message);
                break;
            default:
                break;
        }
    }

    private void handleRenameMessage(Session session, MessageContainer message) {

    }

    private void handleChatMessage(Session session, MessageContainer message) {
        Chat chat = null;
        try {
            chat = gson.fromJson(message.getMessage(), Chat.class);
            chat.setUserName(RoomHandler.getInstance().getUserForSession(session).getName());

            message.setMessage(gson.toJsonTree(chat));
            handler.broadcast(RoomHandler.getInstance().getRoomForSession(session).getUsers(), message);
        } catch (Exception exi) {
            exi.printStackTrace();
        }
        LOGGER.info(chat);

    }

}
