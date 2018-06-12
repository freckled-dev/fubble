
package com.freckles.of.couple.fubble;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.junit.Before;

import com.freckles.of.couple.fubble.proto.WebContainer.JoinRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer;

public class WebsocketTest {

    private static final Logger          LOGGER     = LogManager.getLogger(WebsocketTest.class);

    protected List<FubbleClientEndpoint> allClients = new ArrayList<>();

    public List<FubbleClientEndpoint> createClients(int numClients) {
        List<FubbleClientEndpoint> endpoints = new ArrayList<>();

        String serverLocation = FubbleTestProperties.getInstance().getServerLocation();

        for (int index = 0; index < numClients; index++) {
            try {
                FubbleClientEndpoint clientEndPoint = new FubbleClientEndpoint(new URI(serverLocation));
                endpoints.add(clientEndPoint);
                allClients.add(clientEndPoint);
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
            allClients.add(clientEndPoint);
            return clientEndPoint;
        } catch (URISyntaxException ex) {
            ex.printStackTrace();
        }

        return null;
    }

    public void joinRoom(String roomName, FubbleClientEndpoint client) {
        joinRoom(roomName, client, "");
    }

    public void joinRoom(String roomName, FubbleClientEndpoint client, String password) {
        try {
            JoinRoom joinRoom = JoinRoom.newBuilder() //
                .setRoomName(roomName)//
                .setPassword(password) //
                .build();

            MessageContainerServer container = MessageContainerServer.newBuilder().setJoinRoom(joinRoom).build();

            ByteArrayOutputStream output = new ByteArrayOutputStream();
            container.writeTo(output);

            client.sendMessage(output.toByteArray());

            output.close();
        } catch (IOException ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        }
    }

    public void sendMessage(MessageContainerServer message, FubbleClientEndpoint client) {
        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            message.writeTo(output);

            client.sendMessage(output.toByteArray());

            output.close();
        } catch (IOException ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        }
    }

    @Before
    public void tearDown() {
        for (FubbleClientEndpoint client : allClients) {
            if (client.getUserSession() != null) {
                try {
                    client.getUserSession().close();
                } catch (IOException ex) {
                    LOGGER.error(ex);
                }
            }
        }
    }

}
