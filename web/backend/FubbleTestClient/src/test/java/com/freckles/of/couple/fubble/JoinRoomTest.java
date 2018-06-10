
package com.freckles.of.couple.fubble;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.junit.Test;

import com.freckles.of.couple.fubble.proto.WebContainer.JoinedRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient.MessageTypeCase;
import com.freckles.of.couple.fubble.proto.WebContainer.UserJoined;

public class JoinRoomTest extends WebsocketTest {

    private static final Logger LOGGER         = LogManager.getLogger(JoinRoomTest.class);

    private static final String ROOM_NAME_1    = "kugel_test1";
    private static final String ROOM_NAME_2    = "kugel_test2";

    private static final int    WAITING_PERIOD = 1000;

    @Test
    public void testReceiveJoinedRoom() {
        try {
            // 1. client joins
            FubbleClientEndpoint client1 = createClient();
            joinRoom(ROOM_NAME_1, client1);

            Thread.sleep(WAITING_PERIOD);

            // check if he received a joined room msg for himself
            boolean joinedRoomReceived = false;

            for (MessageContainerClient message : client1.getMessages()) {
                MessageTypeCase type = message.getMessageTypeCase();
                if (MessageTypeCase.JOINED_ROOM.equals(type)) {
                    JoinedRoom joined = message.getJoinedRoom();
                    if (joined.getUserId().equals(client1.getUserId())) {
                        joinedRoomReceived = true;
                    }
                }
            }

            assertTrue(joinedRoomReceived);

            Thread.sleep(WAITING_PERIOD);

            // 2. client joins
            FubbleClientEndpoint client2 = createClient();
            joinRoom(ROOM_NAME_1, client2);

            Thread.sleep(WAITING_PERIOD);

            // check if he received a joined room msg for himself
            joinedRoomReceived = false;

            for (MessageContainerClient message : client2.getMessages()) {
                MessageTypeCase type = message.getMessageTypeCase();
                if (MessageTypeCase.JOINED_ROOM.equals(type)) {
                    JoinedRoom joined = message.getJoinedRoom();
                    if (joined.getUserId().equals(client2.getUserId())) {
                        joinedRoomReceived = true;
                    }
                }
            }

            assertTrue(joinedRoomReceived);

            Thread.sleep(WAITING_PERIOD);

        } catch (Exception ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        }
    }

    @Test
    public void testReceiveUserJoined() {
        try {
            // 1. client joins
            FubbleClientEndpoint client1 = createClient();
            joinRoom(ROOM_NAME_2, client1);

            Thread.sleep(WAITING_PERIOD);

            // check if he received a user joined msg for himself
            for (MessageContainerClient message : client1.getMessages()) {
                MessageTypeCase type = message.getMessageTypeCase();
                if (MessageTypeCase.USER_JOINED.equals(type)) {
                    UserJoined joined = message.getUserJoined();
                    assertEquals(client1.getUserId(), joined.getUserId());
                }
            }

            Thread.sleep(WAITING_PERIOD);

            // 2. client joins
            FubbleClientEndpoint client2 = createClient();
            joinRoom(ROOM_NAME_2, client2);

            Thread.sleep(WAITING_PERIOD);

            // check if client2 received a user joined msg for himself
            boolean received = false;
            for (MessageContainerClient message : client2.getMessages()) {
                MessageTypeCase type = message.getMessageTypeCase();
                if (MessageTypeCase.USER_JOINED.equals(type)) {
                    UserJoined joined = message.getUserJoined();

                    if (client2.getUserId().equals(joined.getUserId())) {
                        received = true;
                        break;
                    }
                }
            }
            assertTrue(received);

            // check if client2 received a user joined msg for client1
            received = false;
            for (MessageContainerClient message : client2.getMessages()) {
                MessageTypeCase type = message.getMessageTypeCase();
                if (MessageTypeCase.USER_JOINED.equals(type)) {
                    UserJoined joined = message.getUserJoined();

                    if (client1.getUserId().equals(joined.getUserId())) {
                        received = true;
                        break;
                    }
                }
            }
            assertTrue(received);

            // check if client1 received a user joined msg for client2
            received = false;
            for (MessageContainerClient message : client1.getMessages()) {
                MessageTypeCase type = message.getMessageTypeCase();
                if (MessageTypeCase.USER_JOINED.equals(type)) {
                    UserJoined joined = message.getUserJoined();

                    if (client2.getUserId().equals(joined.getUserId())) {
                        received = true;
                        break;
                    }
                }
            }
            assertTrue(received);

            Thread.sleep(WAITING_PERIOD);

            // 3. client joins
            FubbleClientEndpoint client3 = createClient();
            joinRoom(ROOM_NAME_2, client3);

            Thread.sleep(WAITING_PERIOD);

            // check if client3 received a user joined msg for himself
            received = false;
            for (MessageContainerClient message : client3.getMessages()) {
                MessageTypeCase type = message.getMessageTypeCase();
                if (MessageTypeCase.USER_JOINED.equals(type)) {
                    UserJoined joined = message.getUserJoined();

                    if (client3.getUserId().equals(joined.getUserId())) {
                        received = true;
                        break;
                    }
                }
            }
            assertTrue(received);

            // check if client3 received a user joined msg for client1 + client2
            boolean received1 = false;
            boolean received2 = false;
            for (MessageContainerClient message : client3.getMessages()) {
                MessageTypeCase type = message.getMessageTypeCase();
                if (MessageTypeCase.USER_JOINED.equals(type)) {
                    UserJoined joined = message.getUserJoined();

                    if (client1.getUserId().equals(joined.getUserId())) {
                        received1 = true;
                    }
                    if (client2.getUserId().equals(joined.getUserId())) {
                        received2 = true;
                    }
                }
            }
            assertTrue(received1);
            assertTrue(received2);

            // check if client1 received a user joined msg for client3
            received = false;
            for (MessageContainerClient message : client1.getMessages()) {
                MessageTypeCase type = message.getMessageTypeCase();
                if (MessageTypeCase.USER_JOINED.equals(type)) {
                    UserJoined joined = message.getUserJoined();

                    if (client3.getUserId().equals(joined.getUserId())) {
                        received = true;
                        break;
                    }
                }
            }
            assertTrue(received);

            // check if client2 received a user joined msg for client3
            received = false;
            for (MessageContainerClient message : client2.getMessages()) {
                MessageTypeCase type = message.getMessageTypeCase();
                if (MessageTypeCase.USER_JOINED.equals(type)) {
                    UserJoined joined = message.getUserJoined();

                    if (client3.getUserId().equals(joined.getUserId())) {
                        received = true;
                        break;
                    }
                }
            }
            assertTrue(received);

            Thread.sleep(WAITING_PERIOD);

        } catch (Exception ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        }
    }

}
