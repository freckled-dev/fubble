
package com.freckles.of.couple.fubble;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import java.util.List;
import java.util.stream.Collectors;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.junit.Assert;
import org.junit.Test;

import com.freckles.of.couple.fubble.helper.NumResponseCalculator;

public class NamingUsersTest extends WebsocketTestRunner {

    private static final Logger LOGGER      = LogManager.getLogger(NamingUsersTest.class);
    private static final String ROOM_NAME_1 = "martin-loves-games-test1";
    private static final String ROOM_NAME_2 = "martin-loves-games-test2";

    @Test
    public void testNamingSimple() {
        List<FubbleClientEndpoint> allClients = createClients(5);

        try {
            for (int index = 0; index < allClients.size(); index++) {
                FubbleClientEndpoint client = allClients.get(index);
                ResponseCount.startLatch(NumResponseCalculator.numJoinRoomResponses(index));
                joinRoom(ROOM_NAME_1, client);
                ResponseCount.await();
            }

            // check if they are named correctly
            List<String> userNames = allClients.stream().map(FubbleClientEndpoint::getUserName).collect(Collectors.toList());
            assertEquals(5, userNames.size());

            for (int index = 0; index < allClients.size(); index++) {
                assertTrue(userNames.contains(FubbleTestProperties.getInstance().getUserNamePrefix() + (index + 1)));
            }

        } catch (Exception ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
            Assert.fail();
        }
    }

    @Test
    public void testNamingExtended() {
        try {
            // 5 clients join the same room - Fubbler1, Fubbler2, Fubbler3, Fubbler4, Fubbler5
            createClients(5);

            for (int index = 0; index < allClients.size(); index++) {
                FubbleClientEndpoint client = allClients.get(index);
                ResponseCount.startLatch(NumResponseCalculator.numJoinRoomResponses(index));
                joinRoom(ROOM_NAME_2, client);
                ResponseCount.await();
            }

            // Fubbler4, Fubbler5 leave
            FubbleClientEndpoint fubbler4 = allClients.get(3);
            ResponseCount.startLatch(NumResponseCalculator.numUserLeftResponses(4));
            fubbler4.getUserSession().close();
            ResponseCount.await();

            FubbleClientEndpoint fubbler5 = allClients.get(4);
            ResponseCount.startLatch(NumResponseCalculator.numUserLeftResponses(3));
            fubbler5.getUserSession().close();
            ResponseCount.await();

            allClients.remove(fubbler5);
            allClients.remove(fubbler4);

            // 2 join - Fubbler6, Fubbler7
            List<FubbleClientEndpoint> secondJoins = createClients(2);
            ResponseCount.startLatch(NumResponseCalculator.numJoinRoomResponses(3));
            joinRoom(ROOM_NAME_2, secondJoins.get(0));
            ResponseCount.await();
            ResponseCount.startLatch(NumResponseCalculator.numJoinRoomResponses(4));
            joinRoom(ROOM_NAME_2, secondJoins.get(1));
            ResponseCount.await();

            // check if they are named correctly
            List<String> userNames = allClients.stream().map(FubbleClientEndpoint::getUserName).collect(Collectors.toList());
            assertTrue(userNames.contains(FubbleTestProperties.getInstance().getUserNamePrefix() + "1"));
            assertTrue(userNames.contains(FubbleTestProperties.getInstance().getUserNamePrefix() + "2"));
            assertTrue(userNames.contains(FubbleTestProperties.getInstance().getUserNamePrefix() + "3"));
            assertTrue(userNames.contains(FubbleTestProperties.getInstance().getUserNamePrefix() + "6"));
            assertTrue(userNames.contains(FubbleTestProperties.getInstance().getUserNamePrefix() + "7"));

        } catch (Exception ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
            Assert.fail();
        }
    }

}
