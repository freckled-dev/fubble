import static org.junit.Assert.assertTrue;

import java.io.ByteArrayOutputStream;
import java.io.IOException;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.junit.Test;

import com.freckles.of.couple.fubble.FubbleClientEndpoint;
import com.freckles.of.couple.fubble.proto.WebContainer.JoinRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.JoinedRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient.MessageTypeCase;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer;

public class JoinRoomTest extends FubbleWebSocketTest {

    private static final String ROOM_NAME_1    = "kugel_test1";

    private static final Logger LOGGER         = LogManager.getLogger(JoinRoomTest.class);

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
                if (MessageTypeCase.JOINEDROOM.equals(type)) {
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
                if (MessageTypeCase.JOINEDROOM.equals(type)) {
                    JoinedRoom joined = message.getJoinedRoom();
                    if (joined.getUserId().equals(client2.getUserId())) {
                        joinedRoomReceived = true;
                    }
                }
            }

            assertTrue(joinedRoomReceived);

            Thread.sleep(WAITING_PERIOD);

        } catch (Exception ex) {
            LOGGER.error(ex);
        }
    }

    private void joinRoom(String roomName, FubbleClientEndpoint client) {
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

}
