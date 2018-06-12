
package com.freckles.of.couple.fubble;

import static org.junit.Assert.assertEquals;

import java.util.List;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.junit.Test;

import com.freckles.of.couple.fubble.proto.WebContainer.ChatMessage;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient.MessageTypeCase;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer;

public class ChatMessageTest extends WebsocketTest {

    private static final Logger LOGGER         = LogManager.getLogger(ChatMessageTest.class);

    private static final String ROOM_NAME_1    = "manu_test1";

    private static final int    WAITING_PERIOD = 1000;

    @Test
    public void testBroadcastChatMessage() {
        try {
            // 5. client joins
            List<FubbleClientEndpoint> clients = createClients(5);
            for (FubbleClientEndpoint client : clients) {
                joinRoom(ROOM_NAME_1, client);
            }

            Thread.sleep(WAITING_PERIOD);

            // Fubbler2 sends a chat message
            FubbleClientEndpoint fubbler2 = clients.stream().filter(client -> client.getUserName().equals("Fubbler2")).findFirst().get();
            String greetings = "Good morning everyone!";
            ChatMessage chatMessage = ChatMessage.newBuilder().setContent(greetings).build();
            MessageContainerServer message = MessageContainerServer.newBuilder().setChatMessage(chatMessage).build();
            sendMessage(message, fubbler2);

            Thread.sleep(WAITING_PERIOD);

            // check if everyone received the message
            for (FubbleClientEndpoint client : clients) {
                MessageContainerClient chatMsg = client.getMessages().stream()//
                    .filter(msg -> msg.getMessageTypeCase().equals(MessageTypeCase.CHAT_MESSAGE)) //
                    .findFirst().get();
                assertEquals(greetings, chatMsg.getChatMessage().getContent());
            }

        } catch (Exception ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        }
    }

}
