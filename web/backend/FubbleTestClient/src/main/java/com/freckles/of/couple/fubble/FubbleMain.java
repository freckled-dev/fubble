
package com.freckles.of.couple.fubble;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.InputStreamReader;
import java.net.URI;
import java.util.Arrays;
import java.util.List;

import com.freckles.of.couple.fubble.proto.WebContainer.ChatMessage;
import com.freckles.of.couple.fubble.proto.WebContainer.JoinRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.LockRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer;
import com.freckles.of.couple.fubble.proto.WebContainer.RenameUser;

public class FubbleMain {

    private static final List<String>   PARAMS_QUIT  = Arrays.asList("q", "quit", "exit");
    private static final List<String>   PARAMS_HELP  = Arrays.asList("h", "help", "info");
    private static final String         PARAM_RENAME = "rename";
    private static final String         PARAM_LOCK   = "lock";

    private static FubbleClientEndpoint client;

    public static void main(String[] args) {
        String serverLocation = FubbleTestProperties.getInstance().getServerLocation();

        try {
            client = new FubbleClientEndpoint(new URI(serverLocation));
            client.setPrintSysout(true);

            joinRoom("we_are_testing");

            BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

            while (true) {
                String input = br.readLine();
                input = input.trim();

                if (PARAMS_QUIT.contains(input)) {
                    executeExit();
                    continue;
                }

                if (PARAMS_HELP.contains(input)) {
                    showInfo();
                    continue;
                }

                if (input.contains(PARAM_RENAME)) {
                    renameUser(input);
                    continue;
                }

                if (input.contains(PARAM_LOCK)) {
                    lockRoom(input);
                    continue;
                }

                sendChatMessage(input);

            }

        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    private static void lockRoom(String lockInfo) {
        String[] split = lockInfo.split(" ");
        if (split.length != 2 || !split[0].trim().equals(PARAM_LOCK)) {
            showInfo();
            return;
        }

        try {
            boolean lock = Integer.parseInt(split[1]) == 1;

            LockRoom lockRoom = LockRoom.newBuilder().setLock(lock).build();
            MessageContainerServer container = MessageContainerServer.newBuilder().setLockRoom(lockRoom).build();

            sendMessage(container);
        } catch (NumberFormatException ex) {
            showInfo();
            return;
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
        if (!split[0].trim().equals(PARAM_RENAME)) {
            showInfo();
            return;
        }

        String newName = input.replace(PARAM_RENAME, "").trim();
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
        System.out.println("lock 0|1 ... unlock and lock the room");
    }

    private static void executeExit() {
        System.out.println("Exit!");
        System.exit(0);
    }

}
