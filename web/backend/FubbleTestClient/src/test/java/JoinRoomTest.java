import java.io.ByteArrayOutputStream;
import java.io.IOException;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.junit.Test;

import com.freckles.of.couple.fubble.proto.WebContainer.JoinRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainer;

public class JoinRoomTest extends FubbleWebSocketTest {

    private static final Logger LOGGER = LogManager.getLogger(JoinRoomTest.class);

    @Test
    public void testJoinRoom() {
        MessageContainer joinRoom = createJoinRoomContainer();
        ByteArrayOutputStream output = new ByteArrayOutputStream();
        try {
            joinRoom.writeTo(output);
            clientEndPoint.sendMessage(output.toByteArray());
            output.close();
        } catch (IOException ex) {
            LOGGER.error(ex);
        }
    }

    private static MessageContainer createJoinRoomContainer() {
        JoinRoom joinRoom = JoinRoom.newBuilder().setRoomName("martin-loves-dick").build();
        return MessageContainer.newBuilder().setJoinRoom(joinRoom).build();
    }

}
