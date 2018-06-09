package com.freckles.of.couple.fubble;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.List;

import com.freckles.of.couple.fubble.FubbleClientEndpoint;
import com.freckles.of.couple.fubble.FubbleTestProperties;

public class ClientCreator {

    public List<FubbleClientEndpoint> createClients(int numClients) {
        List<FubbleClientEndpoint> endpoints = new ArrayList<>();

        String serverLocation = FubbleTestProperties.getInstance().getServerLocation();

        for (int index = 0; index < numClients; index++) {
            try {
                FubbleClientEndpoint clientEndPoint = new FubbleClientEndpoint(new URI(serverLocation));
                endpoints.add(clientEndPoint);
            } catch (URISyntaxException ex) {
                ex.printStackTrace();
            }
        }

        return endpoints;
    }

    public FubbleClientEndpoint createClient() {
        String serverLocation = FubbleTestProperties.getInstance().getServerLocation();

        try {
            FubbleClientEndpoint clientEndPoint = new FubbleClientEndpoint(new URI(serverLocation));
            return clientEndPoint;
        } catch (URISyntaxException ex) {
            ex.printStackTrace();
        }

        return null;
    }

}
