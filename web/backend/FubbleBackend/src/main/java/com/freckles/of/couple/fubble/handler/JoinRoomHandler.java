
package com.freckles.of.couple.fubble.handler;

import java.io.ByteArrayOutputStream;
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
        User user = joinRoom(room);

        // 1. Send JoinedRoom for the new user
        sendJoinedRoom(room, user);

        // 2. Send UserJoined for the existing users to the new user
        broadcastExisting(room, user);

        // 3. Send Broadcast UserJoined to all users in the room
        broadcastUserJoined(room, user);

    }

    private void broadcastExisting(Room room, User user) {
        List<User> allUsers = new ArrayList<>();
        allUsers.addAll(room.getUsers());
        allUsers.remove(user);

        for (User currentUser : allUsers) {
            UserJoined userJoined = UserJoined.newBuilder() //
                .setId(currentUser.getId()) //
                .setName(currentUser.getName()) //
                .build();

            MessageContainerClient clientMsg = MessageContainerClient.newBuilder().setUserJoined(userJoined).build();

            try {
                ByteArrayOutputStream output = new ByteArrayOutputStream();
                clientMsg.writeTo(output);
                messageHelper.sendAsync(user.getSession(), output.toByteArray());
                output.close();
            } catch (Exception ex) {
                LOGGER.error(ex);
            }
        }
    }

    private void sendJoinedRoom(Room room, User user) {
        JoinedRoom joinedRoom = JoinedRoom.newBuilder() //
            .setRoomId(room.getId()) //
            .setUserId(user.getId()) //
            .build();

        MessageContainerClient clientMsg = MessageContainerClient.newBuilder().setJoinedRoom(joinedRoom).build();

        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            clientMsg.writeTo(output);
            messageHelper.sendAsync(user.getSession(), output.toByteArray());
            output.close();
        } catch (Exception ex) {
            LOGGER.error(ex);
        }
    }

    private void broadcastUserJoined(Room room, User user) {
        UserJoined userJoined = UserJoined.newBuilder() //
            .setId(user.getId()) //
            .setName(user.getName()) //
            .build();

        MessageContainerClient clientMsg = MessageContainerClient.newBuilder().setUserJoined(userJoined).build();

        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            clientMsg.writeTo(output);
            List<Session> sessions = room.getUsers().stream().map(User::getSession).collect(Collectors.toList());
            messageHelper.broadcastAsync(sessions, output.toByteArray());
            output.close();
        } catch (Exception ex) {
            LOGGER.error(ex);
        }
    }

    private Room getRoom(String roomName) {
        Room room = roomDAO.readOrCreateRoom(roomName);
        this.connection.setRoom(room);
        return room;
    }

    private User joinRoom(Room room) {
        User user = userHandler.createUser(connection, room.getConnectedCounter());
        room.setConnectedCounter(room.getConnectedCounter() + 1);
        room.getUsers().add(user);
        LOGGER.info(String.format("Server: The user %s has entered the room %s.", user.getName(), room.getName()));
        return user;
    }

}
