
package com.freckles.of.couple.fubble;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.junit.Test;

public class NamingUsersTest extends WebsocketTest {

    private static final Logger LOGGER         = LogManager.getLogger(NamingUsersTest.class);
    private static final String ROOM_NAME_1    = "martin-loves-games-test1";
    private static final String ROOM_NAME_2    = "martin-loves-games-test2";

    private static final int    WAITING_PERIOD = 1000;

    @Test
    public void testNamingSimple() {
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

        } catch (InterruptedException ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        }
    }

    @Test
    public void testNamingExtended() {
        List<FubbleClientEndpoint> allClients = new ArrayList<>();

        try {
            // 5 clients join the same room - Fubbler1, Fubbler2, Fubbler3, Fubbler4, Fubbler5
            List<FubbleClientEndpoint> firstJoins = createClients(5);
            allClients.addAll(firstJoins);

            for (FubbleClientEndpoint client : firstJoins) {
                joinRoom(ROOM_NAME_2, client);
            }

            Thread.sleep(WAITING_PERIOD);

            // Fubbler4, Fubbler5 leave
            List<FubbleClientEndpoint> leaving = firstJoins.stream().filter(
                client -> "Fubbler4".equals(client.getUserName()) || "Fubbler5".equals(client.getUserName())).collect(Collectors.toList());
            for (FubbleClientEndpoint leave : leaving) {
                leave.getUserSession().close();
            }

            Thread.sleep(WAITING_PERIOD);

            // 2 join - Fubbler6, Fubbler7
            List<FubbleClientEndpoint> secondJoins = createClients(2);
            allClients.addAll(secondJoins);

            for (FubbleClientEndpoint client : secondJoins) {
                joinRoom(ROOM_NAME_2, client);
            }

            Thread.sleep(WAITING_PERIOD);

            // check if they are named correctly
            List<String> userNames = allClients.stream().map(FubbleClientEndpoint::getUserName).collect(Collectors.toList());
            LOGGER.info(userNames);
            for (int index = 0; index < allClients.size(); index++) {
                assertTrue(userNames.contains(FubbleTestProperties.getInstance().getUserNamePrefix() + (index + 1)));
            }

        } catch (InterruptedException | IOException ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        }
    }

}