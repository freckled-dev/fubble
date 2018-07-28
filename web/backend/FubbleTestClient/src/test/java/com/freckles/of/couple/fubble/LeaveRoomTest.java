
package com.freckles.of.couple.fubble;

import static org.junit.Assert.assertTrue;

import java.util.List;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.junit.Assert;
import org.junit.Test;

import com.freckles.of.couple.fubble.helper.NumResponseCalculator;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient.MessageTypeCase;
import com.freckles.of.couple.fubble.proto.WebContainer.UserLeft;

public class LeaveRoomTest extends WebsocketTestRunner {

    private static final Logger LOGGER      = LogManager.getLogger(LeaveRoomTest.class);

    private static final String ROOM_NAME_1 = "martin_test1";

    @Test
    public void testReceiveUserLeft() {
        try {
            // 5 clients join
            List<FubbleClientEndpoint> clients = createClients(5);

            for (int index = 0; index < clients.size(); index++) {
                FubbleClientEndpoint client = clients.get(index);
                ResponseCount.startLatch(NumResponseCalculator.numJoinRoomResponses(index));
                joinRoom(ROOM_NAME_1, client);
                ResponseCount.await();
            }

            // 2 clients leave (Fubbler2 + Fubbler4) - check if the other 3 get the user left messages
            FubbleClientEndpoint fubbler2 = clients.get(1);
            ResponseCount.startLatch(NumResponseCalculator.numUserLeftResponses(4));
            fubbler2.getUserSession().close();
            ResponseCount.await();
            allClients.remove(fubbler2);

            FubbleClientEndpoint fubbler4 = clients.get(3);
            ResponseCount.startLatch(NumResponseCalculator.numUserLeftResponses(3));
            fubbler4.getUserSession().close();
            ResponseCount.await();
            allClients.remove(fubbler4);

            String fubbler2_id = fubbler2.getUserId();
            String fubbler4_id = fubbler4.getUserId();

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
            for (MessageContainerClient message : fubbler4.getMessages()) {
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

        } catch (Exception ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
            Assert.fail();
        }
    }

}
