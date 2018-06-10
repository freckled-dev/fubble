
package com.freckles.of.couple.fubble;

import static org.junit.Assert.assertTrue;

import java.util.List;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.junit.Test;

import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient.MessageTypeCase;
import com.freckles.of.couple.fubble.proto.WebContainer.UserLeft;

public class LeaveRoomTest extends WebsocketTest {

    private static final Logger LOGGER         = LogManager.getLogger(LeaveRoomTest.class);

    private static final String ROOM_NAME_1    = "martin_test1";

    private static final int    WAITING_PERIOD = 1000;

    @Test
    public void testReceiveUserLeft() {
        try {
            // 5 clients join
            List<FubbleClientEndpoint> clients = createClients(5);

            for (FubbleClientEndpoint client : clients) {
                joinRoom(ROOM_NAME_1, client);
            }

            Thread.sleep(WAITING_PERIOD);

            // 2 clients leave (Fubbler2 + Fubbler4) - check if the other 3 get the user left messages
            clients.get(1).getUserSession().close();
            Thread.sleep(WAITING_PERIOD);
            clients.get(3).getUserSession().close();

            String fubbler2_id = clients.get(1).getUserId();
            String fubbler4_id = clients.get(3).getUserId();

            Thread.sleep(WAITING_PERIOD);

            // check Fubbler1
            boolean fubbler2LeftReceived = false;
            boolean fubbler4LeftReceived = false;
            for (MessageContainerClient message : clients.get(0).getMessages()) {
                if (MessageTypeCase.USER_LEFT.equals(message.getMessageTypeCase())) {
                    UserLeft userLeft = message.getUserLeft();
                    if (userLeft.getUserId().equals(fubbler2_id)) {
                        fubbler2LeftReceived = true;
                    }
                    if (userLeft.getUserId().equals(fubbler4_id)) {
                        fubbler4LeftReceived = true;
                    }
                }
            }
            assertTrue(fubbler2LeftReceived);
            assertTrue(fubbler4LeftReceived);

            // check Fubbler4 (Fubbler2 left first - so Fubbler4 must also get a message before he leaves)
            fubbler2LeftReceived = false;
            for (MessageContainerClient message : clients.get(3).getMessages()) {
                if (MessageTypeCase.USER_LEFT.equals(message.getMessageTypeCase())) {
                    UserLeft userLeft = message.getUserLeft();
                    if (userLeft.getUserId().equals(fubbler2_id)) {
                        fubbler2LeftReceived = true;
                    }
                }
            }
            assertTrue(fubbler2LeftReceived);

            // check fubbler 3
            fubbler2LeftReceived = false;
            fubbler4LeftReceived = false;
            for (MessageContainerClient message : clients.get(2).getMessages()) {
                if (MessageTypeCase.USER_LEFT.equals(message.getMessageTypeCase())) {
                    UserLeft userLeft = message.getUserLeft();
                    if (userLeft.getUserId().equals(fubbler2_id)) {
                        fubbler2LeftReceived = true;
                    }
                    if (userLeft.getUserId().equals(fubbler4_id)) {
                        fubbler4LeftReceived = true;
                    }
                }
            }
            assertTrue(fubbler2LeftReceived);
            assertTrue(fubbler4LeftReceived);

            // check fubbler 5
            fubbler2LeftReceived = false;
            fubbler4LeftReceived = false;
            for (MessageContainerClient message : clients.get(4).getMessages()) {
                if (MessageTypeCase.USER_LEFT.equals(message.getMessageTypeCase())) {
                    UserLeft userLeft = message.getUserLeft();
                    if (userLeft.getUserId().equals(fubbler2_id)) {
                        fubbler2LeftReceived = true;
                    }
                    if (userLeft.getUserId().equals(fubbler4_id)) {
                        fubbler4LeftReceived = true;
                    }
                }
            }
            assertTrue(fubbler2LeftReceived);
            assertTrue(fubbler4LeftReceived);

            Thread.sleep(WAITING_PERIOD);

        } catch (Exception ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        }
    }

}
