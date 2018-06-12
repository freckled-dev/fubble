
package com.freckles.of.couple.fubble.handler;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.freckles.of.couple.dal.RoomManagementImpl;
import com.freckles.of.couple.fubble.FubbleEndpoint;
import com.freckles.of.couple.fubble.entities.Room;
import com.freckles.of.couple.fubble.entities.User;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.UserLeft;
import com.freckles.of.couple.fubble.tools.MessageHelper;

public class CloseHandler {

    private static final Logger LOGGER        = LogManager.getLogger(CloseHandler.class);

    private RoomManagementImpl  roomDAO       = new RoomManagementImpl();
    private MessageHelper       messageHelper = new MessageHelper();

    public void handleClose(FubbleEndpoint connection) {
        Room room = connection.getRoom();
        User user = connection.getUser();

        if (room == null || user == null) {
            return;
        }

        // 1. Remove user from room
        removeUser(room, connection);

        // 2. Remove room, if no more user is present
        removeRoom(room);

        // 3. Send UserLeft to remaining users in the room
        broadcastUserLeft(room, user);
    }

    private void broadcastUserLeft(Room room, User user) {
        if (user == null) {
            LOGGER.error(room.getName());
        }
        UserLeft userLeft = UserLeft.newBuilder() //
            .setUserId(user.getId()) //
            .build();

        MessageContainerClient clientMsg = MessageContainerClient.newBuilder().setUserLeft(userLeft).build();
        messageHelper.broadcastAsync(room, clientMsg);
    }

    private void removeRoom(Room room) {
        if (room == null) {
            return;
        }
        if (room.getUsers().isEmpty()) {
            roomDAO.deleteRoom(room.getName());
        }
    }

    private void removeUser(Room room, FubbleEndpoint connection) {
        User user = connection.getUser();

        if (user != null) {
            room.getUsers().remove(user);
            LOGGER.info(String.format("Server: user %s has been removed from room %s.", user.getName(), room.getName()));
        }
    }

}
