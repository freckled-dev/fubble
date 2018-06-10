
package com.freckles.of.couple.fubble.handler;

import java.util.List;
import java.util.stream.Collectors;

import javax.websocket.Session;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.freckles.of.couple.fubble.FubbleEndpoint;
import com.freckles.of.couple.fubble.entities.Room;
import com.freckles.of.couple.fubble.entities.User;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer;
import com.freckles.of.couple.fubble.proto.WebContainer.RenameUser;
import com.freckles.of.couple.fubble.proto.WebContainer.RenamedUser;
import com.freckles.of.couple.fubble.tools.MessageHelper;

public class RenameUserHandler implements FubbleMessageHandler {

    private static final Logger LOGGER        = LogManager.getLogger(RenameUserHandler.class);

    private MessageHelper       messageHelper = new MessageHelper();

    @Override
    public void handleMessage(MessageContainerServer container, FubbleEndpoint connection) {
        RenameUser renameUser = container.getRenameUser();

        // 1. Rename user
        String newName = renameUser.getNewName();
        User user = connection.getRoom().getUsers().stream() //
            .filter(existingUser -> existingUser.getId().equals(connection.getUser().getId())) //
            .findFirst().get();
        String oldName = user.getName();
        user.setName(newName);
        connection.getUser().setName(newName);
        LOGGER.info(String.format("User with id %s has been renamed %s -> %s.", user.getId(), oldName, newName));

        // 2. Broadcast renamed user
        broadcastUserRenamed(connection.getRoom(), user);

    }

    private void broadcastUserRenamed(Room room, User user) {
        RenamedUser renamedUser = RenamedUser.newBuilder() //
            .setUserId(user.getId()) //
            .setNewName(user.getName()) //
            .build();

        MessageContainerClient clientMsg = MessageContainerClient.newBuilder().setRenamedUser(renamedUser).build();
        List<Session> sessions = room.getUsers().stream().map(User::getSession).collect(Collectors.toList());
        messageHelper.broadcastAsync(sessions, clientMsg);

    }

}
