
package com.freckles.of.couple.fubble;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.List;

import javax.websocket.Session;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.junit.After;
import org.junit.Rule;
import org.junit.rules.TestRule;
import org.junit.rules.TestWatcher;
import org.junit.runner.Description;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;

import com.freckles.of.couple.fubble.proto.WebContainer.JoinRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer;

@RunWith(Parameterized.class)
public class WebsocketTest {

    private static final int             NUM_REPEAT_TESTS = 50;

    private static final Logger          LOGGER           = LogManager.getLogger(WebsocketTest.class);

    protected List<FubbleClientEndpoint> allClients       = new ArrayList<>();

    @Parameterized.Parameters
    public static Object[][] data() {
        return new Object[NUM_REPEAT_TESTS][0];
    }

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
        joinRoom(roomName, client, "", "");
    }

    public void joinRoom(String roomName, FubbleClientEndpoint client, String password, String userName) {
        try {
            JoinRoom joinRoom = JoinRoom.newBuilder() //
                .setRoomName(roomName)//
                .setPassword(password) //
                .setUserName(userName) //
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

    @Rule
    public TestRule watcher = new TestWatcher() {
        protected void starting(Description description) {
            System.out.println("\n\n-- Starting test: -- " + description.getMethodName());
        }
    };

    @After
    public void tearDown() {
        for (FubbleClientEndpoint client : allClients) {
            Session session = client.getUserSession();
            if (session != null) {
                try {
                    if (session.isOpen()) {
                        session.close();
                    }
                } catch (IOException ex) {
                    LOGGER.error(ex);
                }
            }
        }
    }

}
