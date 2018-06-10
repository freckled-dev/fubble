
package com.freckles.of.couple.fubble;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.InputStreamReader;
import java.net.URI;
import java.util.Arrays;
import java.util.List;

import com.freckles.of.couple.fubble.proto.WebContainer.ChatMessage;
import com.freckles.of.couple.fubble.proto.WebContainer.JoinRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer;
import com.freckles.of.couple.fubble.proto.WebContainer.RenameUser;

public class FubbleMain {

    private static final List<String>   QUIT_PARAMS  = Arrays.asList("q", "quit", "exit");
    private static final List<String>   HELP_PARAMS  = Arrays.asList("h", "help", "info");
    private static final String         RENAME_PARAM = "rename";

    private static FubbleClientEndpoint client;

    public static void main(String[] args) {
        String serverLocation = FubbleTestProperties.getInstance().getServerLocation();

        try {
            client = new FubbleClientEndpoint(new URI(serverLocation));

            joinRoom("we_are_testing");

            BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

            while (true) {
                String input = br.readLine();
                input = input.trim();

                if (QUIT_PARAMS.contains(input)) {
                    executeExit();
                    continue;
                }

                if (HELP_PARAMS.contains(input)) {
                    showInfo();
                    continue;
                }

                if (input.contains(RENAME_PARAM)) {
                    renameUser(input);
                    continue;
                }

                sendChatMessage(input);

            }

        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    private static void joinRoom(String roomName) {
        JoinRoom joinRoom = JoinRoom.newBuilder().setRoomName(roomName).build();
        MessageContainerServer container = MessageContainerServer.newBuilder().setJoinRoom(joinRoom).build();

        sendMessage(container);
    }

    private static void sendChatMessage(String message) {
        ChatMessage msg = ChatMessage.newBuilder().setContent(message).build();
        MessageContainerServer container = MessageContainerServer.newBuilder().setChatMessage(msg).build();
        sendMessage(container);
    }

    private static void renameUser(String input) {
        String[] split = input.split(" ");
        if (!split[0].trim().equals("rename")) {
            showInfo();
            return;
        }

        String newName = input.replace("rename", "").trim();
        RenameUser renameUser = RenameUser.newBuilder().setNewName(newName).build();
        MessageContainerServer container = MessageContainerServer.newBuilder().setRenameUser(renameUser).build();

        sendMessage(container);
    }

    private static void sendMessage(MessageContainerServer container) {
        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            container.writeTo(output);

            client.sendMessage(output.toByteArray());

            output.close();
        } catch (Exception ex) {

        }
    }

    private static void showInfo() {
        System.out.println("q|quit|exit ... exit the program");
        System.out.println("h|help|info ... shows the help");
        System.out.println("rename {new name} ... rename yourself");
    }

    private static void executeExit() {
        System.out.println("Exit!");
        System.exit(0);
    }

}
