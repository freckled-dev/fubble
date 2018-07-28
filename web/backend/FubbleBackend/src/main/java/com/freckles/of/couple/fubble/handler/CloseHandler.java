
package com.freckles.of.couple.fubble.handler;

import java.util.stream.Collectors;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.freckles.of.couple.dal.RoomManagementImpl;
import com.freckles.of.couple.fubble.FubbleEndpoint;
import com.freckles.of.couple.fubble.entities.Room;
import com.freckles.of.couple.fubble.entities.User;
import com.freckles.of.couple.fubble.grpc.GrpcChannel;
import com.freckles.of.couple.fubble.grpc.RtcGrpc.RtcBlockingStub;
import com.freckles.of.couple.fubble.grpc.RtcMessageContainer.DeleteRoomRequest;
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

        // 2. Send UserLeft to remaining users in the room
        broadcastUserLeft(room, user);

        // 2. Remove room, if no more user is present
        removeRoom(room);

    }

    private void broadcastUserLeft(Room room, User user) {
        if (user == null) {
            LOGGER.error("Could not broadcast userLeft for room " + room.getName());
        }
        UserLeft userLeft = UserLeft.newBuilder() //
            .setUserId(user.getId()) //
            .build();

        MessageContainerClient clientMsg = MessageContainerClient.newBuilder().setUserLeft(userLeft).build();
        int size = room.getUsers().stream().map(User::getSession).collect(Collectors.toList()).size();
        System.out.println(String.format("Broadcast user %s left to %s clients.", user.getName(), size));
        messageHelper.broadcastAsync(room, clientMsg);
    }

    private void removeRoom(Room room) {
        if (room == null) {
            return;
        }

        if (roomIsEmpty(room)) {
            sendRtcDeleteRoom(room);
            deleteRoom(room);
        }
    }

    private void deleteRoom(Room room) {
        roomDAO.deleteRoom(room.getName());
    }

    private boolean roomIsEmpty(Room room) {
        return room.getUsers().isEmpty();
    }

    private void sendRtcDeleteRoom(Room room) {
        RtcBlockingStub blockingStub = GrpcChannel.getInstance().createBlockingStub();
        DeleteRoomRequest deleteRoom = DeleteRoomRequest.newBuilder().setId(room.getId()).build();
        try {
            blockingStub.deleteRoom(deleteRoom);
        } catch (Exception ex) {
            ExceptionUtils.getStackTrace(ex);
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
