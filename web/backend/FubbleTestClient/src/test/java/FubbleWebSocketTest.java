import java.net.URI;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.List;

import com.freckles.of.couple.fubble.FubbleClientEndpoint;
import com.freckles.of.couple.fubble.FubbleTestProperties;

public class FubbleWebSocketTest {

    protected List<FubbleClientEndpoint> clientEndPoints = new ArrayList<>();

    public void createClients(int numClients) {
        String serverLocation = FubbleTestProperties.getInstance().getServerLocation();

        for (int index = 0; index < numClients; index++) {
            try {
                FubbleClientEndpoint clientEndPoint = new FubbleClientEndpoint(new URI(serverLocation));
                clientEndPoints.add(clientEndPoint);
            } catch (URISyntaxException ex) {
                ex.printStackTrace();
            }
        }
    }

}
