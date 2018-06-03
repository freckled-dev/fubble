import java.net.URI;
import java.net.URISyntaxException;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.junit.Before;

import com.freckles.of.couple.fubble.FubbleClientEndpoint;
import com.freckles.of.couple.fubble.FubbleTestProperties;

public class FubbleWebSocketTest {

    private static final Logger    LOGGER = LogManager.getLogger(FubbleWebSocketTest.class);
    protected FubbleClientEndpoint clientEndPoint;

    @Before
    public void setUp() {
        String serverLocation = FubbleTestProperties.getInstance().getServerLocation();

        try {
            clientEndPoint = new FubbleClientEndpoint(new URI(serverLocation));

            clientEndPoint.addMessageHandler(new FubbleClientEndpoint.MessageHandler() {
                public void handleMessage(String message) {
                    LOGGER.info(message);
                }
            });

        } catch (URISyntaxException ex) {
            ex.printStackTrace();
        }
    }

}
