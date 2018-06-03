
package com.freckles.of.couple.fubble.handler;

import javax.websocket.Session;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.freckles.of.couple.dal.RoomManagementDAO;
import com.freckles.of.couple.dal.cache.RoomManagementDAOImpl;
import com.freckles.of.couple.fubble.entities.Room;
import com.freckles.of.couple.fubble.entities.User;
import com.freckles.of.couple.fubble.proto.WebContainer.JoinRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainer;

public class JoinRoomHandler implements FubbleMessageHandler {

    private static final Logger LOGGER      = LogManager.getLogger(JoinRoomHandler.class);

    private RoomManagementDAO   roomDAO     = new RoomManagementDAOImpl();
    private UserHandler         userHandler = new UserHandler();

    @Override
    public void handleMessage(MessageContainer container, Session session) {
        JoinRoom joinRoom = container.getJoinRoom();

        Room room = getRoom(joinRoom.getRoomName());
        joinRoom(room, session);
    }

    private Room getRoom(String roomName) {
        Room room = roomDAO.readRoom(roomName);
        if (room != null) {
            return room;
        }

        return roomDAO.createRoom(roomName);
    }

    private void joinRoom(Room room, Session session) {
        User user = userHandler.createUser(session, room.getConnectedCounter());
        int connectedCounter = room.getConnectedCounter();
        room.setConnectedCounter(connectedCounter++);
        room.getUsers().add(user);
        LOGGER.info(String.format("The user %s has entered the room %s.", user.getName(), room.getName()));
    }

}
