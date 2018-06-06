import java.io.ByteArrayOutputStream;
import java.io.IOException;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.junit.Test;

import com.freckles.of.couple.fubble.FubbleClientEndpoint;
import com.freckles.of.couple.fubble.proto.WebContainer.JoinRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainer;

public class JoinRoomTest extends FubbleWebSocketTest {

    private static final Logger LOGGER = LogManager.getLogger(JoinRoomTest.class);

    @Test
    public void testJoinRoom() {
        try {
            MessageContainer joinRoom = createJoinRoomContainer("martin-loves-dick");
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            joinRoom.writeTo(output);

            for (FubbleClientEndpoint clientEndPoint : clientEndPoints) {
                clientEndPoint.sendMessage(output.toByteArray());
            }

            output.close();

            Thread.sleep(5000);

        } catch (IOException ex) {
            LOGGER.error(ex);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    private static MessageContainer createJoinRoomContainer(String roomName) {
        JoinRoom joinRoom = JoinRoom.newBuilder().setRoomName(roomName).build();
        return MessageContainer.newBuilder().setJoinRoom(joinRoom).build();
    }

}
