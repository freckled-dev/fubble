
package com.freckles.of.couple.fubble.handler;

import java.util.List;
import java.util.stream.Collectors;

import javax.websocket.Session;

import com.freckles.of.couple.fubble.FubbleEndpoint;
import com.freckles.of.couple.fubble.entities.Room;
import com.freckles.of.couple.fubble.entities.User;
import com.freckles.of.couple.fubble.proto.WebContainer.ChatMessage;
import com.freckles.of.couple.fubble.proto.WebContainer.ChatMessageClient;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer;
import com.freckles.of.couple.fubble.tools.MessageHelper;

public class ChatMessageHandler implements FubbleMessageHandler {

    private MessageHelper messageHelper = new MessageHelper();

    @Override
    public void handleMessage(MessageContainerServer container, FubbleEndpoint connection) {
        ChatMessage chatMessage = container.getChatMessage();

        // broadcast to all users
        broadcastChatMessage(connection, chatMessage);
    }

    private void broadcastChatMessage(FubbleEndpoint connection, ChatMessage message) {
        Room room = connection.getRoom();
        User user = connection.getUser();

        ChatMessageClient chatMessage = ChatMessageClient.newBuilder() //
            .setUserId(user.getId()) //
            .setContent(message.getContent()) //
            .build();

        MessageContainerClient clientMsg = MessageContainerClient.newBuilder().setChatMessage(chatMessage).build();
        List<Session> sessions = room.getUsers().stream().map(User::getSession).collect(Collectors.toList());
        messageHelper.broadcastAsync(sessions, clientMsg);
    }

}
