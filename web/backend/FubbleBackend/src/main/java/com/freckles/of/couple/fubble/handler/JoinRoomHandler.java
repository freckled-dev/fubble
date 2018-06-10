
package com.freckles.of.couple.fubble.handler;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

import javax.websocket.Session;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.freckles.of.couple.dal.RoomManagementDAO;
import com.freckles.of.couple.dal.RoomManagementImpl;
import com.freckles.of.couple.fubble.FubbleEndpoint;
import com.freckles.of.couple.fubble.entities.Room;
import com.freckles.of.couple.fubble.entities.User;
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
        User user = getUser(room.getConnectedCounter());

        if (room.isLocked()) {
            handleLockedRoom(room, user);
        }

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

    private void handleLockedRoom(Room room, User user) {
        LOGGER.info(String.format("User %s wants to join the locked room %s.", user.getName(), room.getName()));

        // 1. Inform user that the room is locked
        FubbleError error = FubbleError.newBuilder().setErrorId(ErrorType.ROOM_LOCKED).build();
        MessageContainerClient container = MessageContainerClient.newBuilder().setError(error).build();
        messageHelper.sendAsync(user.getSession(), container);

        // 2 Inform other users that a new user wants to join the locked room

        // 3 one of them needs "allow" for the new user to enter

        // 4 user need so be informed as soon as this happens -> then he can join

    }

    private void broadcastExisting(Room room, User user) {
        List<User> allUsers = new ArrayList<>();
        allUsers.addAll(room.getUsers());
        allUsers.remove(user);

        for (User currentUser : allUsers) {
            UserJoined userJoined = UserJoined.newBuilder() //
                .setUserId(currentUser.getId()) //
                .setUserName(currentUser.getName()) //
                .build();

            MessageContainerClient clientMsg = MessageContainerClient.newBuilder().setUserJoined(userJoined).build();
            messageHelper.sendAsync(user.getSession(), clientMsg);
        }
    }

    private void sendJoinedRoom(Room room, User user) {
        JoinedRoom joinedRoom = JoinedRoom.newBuilder() //
            .setRoomId(room.getId()) //
            .setUserId(user.getId()) //
            .setUserName(user.getName()) //
            .build();

        MessageContainerClient clientMsg = MessageContainerClient.newBuilder().setJoinedRoom(joinedRoom).build();
        messageHelper.sendAsync(user.getSession(), clientMsg);
    }

    private void broadcastUserJoined(Room room, User user) {
        UserJoined userJoined = UserJoined.newBuilder() //
            .setUserId(user.getId()) //
            .setUserName(user.getName()) //
            .build();

        MessageContainerClient clientMsg = MessageContainerClient.newBuilder().setUserJoined(userJoined).build();
        List<Session> sessions = room.getUsers().stream().map(User::getSession).collect(Collectors.toList());
        messageHelper.broadcastAsync(sessions, clientMsg);
    }

    private Room getRoom(String roomName) {
        Room room = roomDAO.readOrCreateRoom(roomName);
        this.connection.setRoom(room);
        return room;
    }

    private void joinRoom(Room room, User user) {
        room.setConnectedCounter(room.getConnectedCounter() + 1);
        room.getUsers().add(user);
        LOGGER.info(String.format("Server: The user %s has entered the room %s.", user.getName(), room.getName()));
    }

}
