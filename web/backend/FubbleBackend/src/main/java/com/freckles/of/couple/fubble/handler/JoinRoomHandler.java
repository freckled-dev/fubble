
package com.freckles.of.couple.fubble.handler;

import java.util.ArrayList;
import java.util.List;

import javax.websocket.Session;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.freckles.of.couple.dal.RoomManagementDAO;
import com.freckles.of.couple.dal.RoomManagementImpl;
import com.freckles.of.couple.fubble.FubbleEndpoint;
import com.freckles.of.couple.fubble.entities.Room;
import com.freckles.of.couple.fubble.entities.User;
import com.freckles.of.couple.fubble.grpc.GrpcChannel;
import com.freckles.of.couple.fubble.grpc.RtcGrpc.RtcBlockingStub;
import com.freckles.of.couple.fubble.grpc.RtcMessageContainer.JoinRoomRequest;
import com.freckles.of.couple.fubble.grpc.RtcMessageContainer.RoomInformation;
import com.freckles.of.couple.fubble.proto.WebContainer.FubbleError;
import com.freckles.of.couple.fubble.proto.WebContainer.FubbleError.ErrorType;
import com.freckles.of.couple.fubble.proto.WebContainer.JoinRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.JoinedRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer;
import com.freckles.of.couple.fubble.proto.WebContainer.UserJoined;
import com.freckles.of.couple.fubble.tools.MessageHelper;

public class JoinRoomHandler implements FubbleMessageHandler {

    private static final Logger LOGGER        = LogManager.getLogger(JoinRoomHandler.class);

    private RoomManagementDAO   roomDAO       = new RoomManagementImpl();
    private UserHandler         userHandler   = new UserHandler();
    private MessageHelper       messageHelper = new MessageHelper();
    private FubbleEndpoint      connection;

    @Override
    public void handleMessage(MessageContainerServer container, FubbleEndpoint connection) {
        this.connection = connection;

        JoinRoom joinRoom = container.getJoinRoom();

        Room room = getRoom(joinRoom.getRoomName());
        this.connection.setRoom(room);

        if (room.isLocked()) {
            if (!room.getPassword().equals(joinRoom.getPassword())) {
                handleLockedRoom(connection.getSession());
                return;
            }
        }

        User user = getUser(room.getConnectedCounter());

        joinRoom(room, user);

        // 1. Send JoinedRoom for the new user
        sendJoinedRoom(room, user);

        // 2. Send UserJoined for the existing users to the new user
        broadcastExisting(room, user);

        // 3. Send Broadcast UserJoined to all users in the room
        broadcastUserJoined(room, user);

    }

    private User getUser(int connectedCounter) {
        return userHandler.createUser(connection, connectedCounter);
    }

    private void handleLockedRoom(Session session) {
        LOGGER.info(String.format("A user tried to join the locked room."));

        // inform user that the room is locked
        FubbleError error = FubbleError.newBuilder().setErrorId(ErrorType.ROOM_LOCKED).build();
        MessageContainerClient errorContainer = MessageContainerClient.newBuilder().setError(error).build();
        messageHelper.sendAsync(session, errorContainer);
    }

    private void broadcastExisting(Room room, User user) {
        List<User> allUsers = new ArrayList<>();
        allUsers.addAll(room.getUsers());
        allUsers.remove(user);

        for (User currentUser : allUsers) {
            UserJoined userJoined = UserJoined.newBuilder() //
                .setUserId(currentUser.getId()) //
                .setUserName(currentUser.getName()) //
                .setAlreadyInRoom(true) //
                .build();

            MessageContainerClient clientMsg = MessageContainerClient.newBuilder().setUserJoined(userJoined).build();
            messageHelper.sendAsync(user.getSession(), clientMsg);
        }
    }

    private void sendJoinedRoom(Room room, User user) {
        String rtcServerUrl = getRtcServerUrl(room);

        JoinedRoom joinedRoom = JoinedRoom.newBuilder() //
            .setRoomId(room.getId()) //
            .setUserId(user.getId()) //
            .setRtcUrl(rtcServerUrl) //
            .setRoomName(room.getName()) //
            .setUserName(user.getName()) //
            .build();

        MessageContainerClient clientMsg = MessageContainerClient.newBuilder().setJoinedRoom(joinedRoom).build();
        messageHelper.sendAsync(user.getSession(), clientMsg);
    }

    private String getRtcServerUrl(Room room) {
        RtcBlockingStub blockingStub = GrpcChannel.getInstance().createBlockingStub();
        JoinRoomRequest joinRoom = JoinRoomRequest.newBuilder().setId(room.getId()).build();
        RoomInformation roomInfo = blockingStub.joinRoom(joinRoom);
        return roomInfo.getRtcServer();
    }

    private void broadcastUserJoined(Room room, User user) {
        UserJoined userJoined = UserJoined.newBuilder() //
            .setUserId(user.getId()) //
            .setUserName(user.getName()) //
            .build();

        MessageContainerClient clientMsg = MessageContainerClient.newBuilder().setUserJoined(userJoined).build();
        messageHelper.broadcastAsync(room, clientMsg);
    }

    private Room getRoom(String roomName) {
        Room room = roomDAO.readOrCreateRoom(roomName);
        return room;
    }

    private void joinRoom(Room room, User user) {
        room.setConnectedCounter(room.getConnectedCounter() + 1);
        room.getUsers().add(user);
        LOGGER.info(String.format("Server: The user %s has entered the room %s.", user.getName(), room.getName()));
    }

}
