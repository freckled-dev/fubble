
package com.freckles.of.couple.fubble;

import java.io.IOException;
import java.util.Date;
import java.util.List;
import java.util.Optional;

import javax.websocket.OnClose;
import javax.websocket.OnError;
import javax.websocket.OnMessage;
import javax.websocket.OnOpen;
import javax.websocket.Session;
import javax.websocket.server.PathParam;
import javax.websocket.server.ServerEndpoint;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.freckles.of.couple.fubble.entities.Chat;
import com.freckles.of.couple.fubble.entities.MessageContainer;
import com.freckles.of.couple.fubble.entities.MessageContainer.MessageType;
import com.freckles.of.couple.fubble.entities.Room;
import com.freckles.of.couple.fubble.entities.User;
import com.freckles.of.couple.fubble.tools.BroadcastHandler;
import com.freckles.of.couple.fubble.tools.MessageDecoder;
import com.freckles.of.couple.fubble.tools.MessageEncoder;
import com.freckles.of.couple.fubble.tools.MessageHandler;
import com.freckles.of.couple.fubble.tools.RoomHandler;
import com.google.gson.Gson;

@ServerEndpoint(value = "/{room}", encoders = MessageEncoder.class, decoders = MessageDecoder.class)
public class FubbleEndpoint {

    private static final Logger LOGGER           = LogManager.getLogger(FubbleEndpoint.class);
    private MessageHandler      messageHandler   = new MessageHandler();
    private BroadcastHandler    broadcastHandler = new BroadcastHandler();
    private Gson                gson             = new Gson();

    @OnOpen
    public void onOpen(Session session, @PathParam("room") String room) {
        LOGGER.info("Log4j: Session openend for room: " + room);

        Room currentRoom = createRoom(room);
        addUserToRoom(session, currentRoom);
    }

    @OnMessage
    public void onMessage(Session session, MessageContainer message)
        throws IOException {
        LOGGER.info("Log4j: Message received: " + message);

        messageHandler.decode(session, message);
    }

    @OnError
    public void onError(Session session, Throwable throwable) {
        // Do error handling here
    }

    @OnClose
    public void onClose(Session session)
        throws IOException {

        User user = RoomHandler.getInstance().getUserForSession(session);
        Room room = RoomHandler.getInstance().getRoomForSession(session);
        room.getUsers().remove(user);

        MessageContainer exitMsg = createExitChatMessage(user);
        broadcastHandler.broadcast(room.getUsers(), exitMsg);
    }

    private MessageContainer createExitChatMessage(User user) {
        MessageContainer exitMsg = new MessageContainer();
        Chat chat = new Chat();
        chat.setTimestamp(new Date());
        chat.setComment(String.format("User %s has disconnected", user.getName()));

        exitMsg.setType(MessageType.CHAT);
        exitMsg.setMessage(gson.toJsonTree(chat));
        return exitMsg;
    }

    private void addUserToRoom(Session session, Room currentRoom) {
        User user = new User();
        user.setSession(session);
        user.setName("Fubbler" + currentRoom.getCounter());
        currentRoom.getUsers().add(user);
        currentRoom.addToCounter();

        broadcastHandler.broadcast(currentRoom.getUsers(), createJoinChatMessage(user));
    }

    private MessageContainer createJoinChatMessage(User user) {
        MessageContainer joinMsg = new MessageContainer();
        Chat chat = new Chat();
        chat.setTimestamp(new Date());
        chat.setComment(String.format("User %s has connected", user.getName()));

        joinMsg.setType(MessageType.CHAT);
        joinMsg.setMessage(gson.toJsonTree(chat));
        return joinMsg;
    }

    private Room createRoom(String roomName) {
        List<Room> rooms = RoomHandler.getInstance().getRooms();
        Optional<Room> existingRoom = rooms.stream().filter(room -> room.getName().equals(roomName)).findFirst();

        if (existingRoom.isPresent()) {
            return existingRoom.get();
        }

        Room newRoom = new Room(roomName);
        RoomHandler.getInstance().getRooms().add(newRoom);
        return newRoom;
    }

}
