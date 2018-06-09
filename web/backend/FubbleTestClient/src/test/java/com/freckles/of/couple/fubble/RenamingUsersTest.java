
package com.freckles.of.couple.fubble;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.util.List;
import java.util.stream.Collectors;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.junit.Test;

import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer;
import com.freckles.of.couple.fubble.proto.WebContainer.RenameUser;
import com.freckles.of.couple.fubble.proto.WebContainer.RenamedUser;

public class RenamingUsersTest extends WebsocketTest {

    private static final Logger LOGGER         = LogManager.getLogger(RenamingUsersTest.class);
    private static final String ROOM_NAME_1    = "hello_markus_1";
    private static final String ROOM_NAME_2    = "hello_markus_2";

    private static final int    WAITING_PERIOD = 1000;

    @Test
    public void testReNaming() {
        List<FubbleClientEndpoint> allClients = createClients(5);

        try {
            // 5 clients join the same room - Fubbler1, Fubbler2, Fubbler3, Fubbler4, Fubbler5
            for (int index = 0; index < allClients.size(); index++) {
                FubbleClientEndpoint client = allClients.get(index);
                joinRoom(ROOM_NAME_1, client);
            }

            Thread.sleep(WAITING_PERIOD);

            // check if they are named correctly
            List<String> userNames = allClients.stream().map(FubbleClientEndpoint::getUserName).collect(Collectors.toList());
            LOGGER.info(userNames);
            assertEquals(5, userNames.size());
            for (int index = 0; index < allClients.size(); index++) {
                assertTrue(userNames.contains(FubbleTestProperties.getInstance().getUserNamePrefix() + (index + 1)));
            }

            Thread.sleep(WAITING_PERIOD);

            // Fubbler2 renames himself to I_am_the_king
            String nameOld = "Fubbler2";
            String nameNew = "I_am_the_king";
            RenameUser renameUser = RenameUser.newBuilder().setName(nameNew).build();
            MessageContainerServer container = MessageContainerServer.newBuilder().setRenameUser(renameUser).build();
            FubbleClientEndpoint fubbler2 = allClients.stream().filter(client -> client.getUserName().equals(nameOld)).findFirst().get();
            sendMessage(container, fubbler2);

            Thread.sleep(WAITING_PERIOD);

            // check if they are named correctly
            userNames = allClients.stream().map(FubbleClientEndpoint::getUserName).collect(Collectors.toList());
            LOGGER.info(userNames);
            assertEquals(5, userNames.size());
            assertTrue(userNames.contains(nameNew));
            assertFalse(userNames.contains(nameOld));

            Thread.sleep(WAITING_PERIOD);

        } catch (Exception ex) {
            LOGGER.error(ex);
        }
    }

    @Test
    public void testReNamingBroadcast() {
        List<FubbleClientEndpoint> allClients = createClients(5);

        try {
            // 5 clients join the same room - Fubbler1, Fubbler2, Fubbler3, Fubbler4, Fubbler5
            for (int index = 0; index < allClients.size(); index++) {
                FubbleClientEndpoint client = allClients.get(index);
                joinRoom(ROOM_NAME_1, client);
            }

            Thread.sleep(WAITING_PERIOD);

            // Fubbler2 renames himself to I_am_the_king
            String nameOld = "Fubbler2";
            String nameNew = "I_am_the_king";
            RenameUser renameUser = RenameUser.newBuilder().setName(nameNew).build();
            MessageContainerServer container = MessageContainerServer.newBuilder().setRenameUser(renameUser).build();
            FubbleClientEndpoint fubbler2 = allClients.stream().filter(client -> client.getUserName().equals(nameOld)).findFirst().get();
            sendMessage(container, fubbler2);

            Thread.sleep(WAITING_PERIOD);

            // check if all of them get the renamedUser Message (including Fubbler2)
            for (FubbleClientEndpoint client : allClients) {
                List<MessageContainerClient> messages = client.getMessages();

                RenamedUser renamed = RenamedUser.newBuilder().setId(fubbler2.getUserId()).setName(nameNew).build();
                MessageContainerClient response = MessageContainerClient.newBuilder().setRenamedUser(renamed).build();
                assertTrue(messages.contains(response));
            }

            Thread.sleep(WAITING_PERIOD);

        } catch (Exception ex) {
            LOGGER.error(ex);
        }
    }

}
