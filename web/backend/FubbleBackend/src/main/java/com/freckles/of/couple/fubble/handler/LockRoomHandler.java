
package com.freckles.of.couple.fubble.handler;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.freckles.of.couple.fubble.FubbleEndpoint;
import com.freckles.of.couple.fubble.entities.Room;
import com.freckles.of.couple.fubble.entities.User;
import com.freckles.of.couple.fubble.proto.WebContainer.LockRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.LockedRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer;
import com.freckles.of.couple.fubble.tools.MessageHelper;

public class LockRoomHandler implements FubbleMessageHandler {

    private static final Logger LOGGER        = LogManager.getLogger(LockRoomHandler.class);

    private MessageHelper       messageHelper = new MessageHelper();

    @Override
    public void handleMessage(MessageContainerServer container, FubbleEndpoint connection) {
        LockRoom lockRoom = container.getLockRoom();

        lockRoom(lockRoom, connection);

        // 2. broadcast the lock
        broadcastLockedRoom(connection);
    }

    private void lockRoom(LockRoom lockRoom, FubbleEndpoint connection) {
        Room room = connection.getRoom();
        room.setPassword(lockRoom.getPassword());

        if (room.isLocked()) {
            LOGGER.info(String.format("Room %s was locked by user %s", connection.getRoom().getName(), connection.getUser().getName()));
        } else {
            LOGGER.info(String.format("Room %s was unlocked by user %s", connection.getRoom().getName(), connection.getUser().getName()));
        }
    }

    private void broadcastLockedRoom(FubbleEndpoint connection) {
        Room room = connection.getRoom();
        User user = connection.getUser();

        LockedRoom lockedRoom = LockedRoom.newBuilder() //
            .setLock(room.isLocked()) //
            .setRoomName(room.getName()) //
            .setUserId(user.getId()) //
            .build();

        MessageContainerClient clientMsg = MessageContainerClient.newBuilder().setLockRoom(lockedRoom).build();
        messageHelper.broadcastAsync(connection.getRoom(), clientMsg);
    }

}