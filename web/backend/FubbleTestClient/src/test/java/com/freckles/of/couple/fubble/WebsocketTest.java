
package com.freckles.of.couple.fubble;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.List;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.freckles.of.couple.fubble.proto.WebContainer.JoinRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer;

public class WebsocketTest {

    private static final Logger LOGGER = LogManager.getLogger(WebsocketTest.class);

    public List<FubbleClientEndpoint> createClients(int numClients) {
        List<FubbleClientEndpoint> endpoints = new ArrayList<>();

        String serverLocation = FubbleTestProperties.getInstance().getServerLocation();

        for (int index = 0; index < numClients; index++) {
            try {
                FubbleClientEndpoint clientEndPoint = new FubbleClientEndpoint(new URI(serverLocation));
                endpoints.add(clientEndPoint);
            } catch (URISyntaxException ex) {
                ex.printStackTrace();
            }
        }

        return endpoints;
    }

    public FubbleClientEndpoint createClient() {
        String serverLocation = FubbleTestProperties.getInstance().getServerLocation();

        try {
            FubbleClientEndpoint clientEndPoint = new FubbleClientEndpoint(new URI(serverLocation));
            return clientEndPoint;
        } catch (URISyntaxException ex) {
            ex.printStackTrace();
        }

        return null;
    }

    public void joinRoom(String roomName, FubbleClientEndpoint client) {
        try {
            JoinRoom joinRoom = JoinRoom.newBuilder().setRoomName(roomName).build();
            MessageContainerServer container = MessageContainerServer.newBuilder().setJoinRoom(joinRoom).build();

            ByteArrayOutputStream output = new ByteArrayOutputStream();
            container.writeTo(output);

            client.sendMessage(output.toByteArray());

            output.close();
        } catch (IOException ex) {
            LOGGER.error(ex);
        }
    }

    public void sendMessage(MessageContainerServer message, FubbleClientEndpoint client) {
        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            message.writeTo(output);

            client.sendMessage(output.toByteArray());

            output.close();
        } catch (IOException ex) {
            LOGGER.error(ex);
        }
    }

}
