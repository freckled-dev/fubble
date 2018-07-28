
package com.freckles.of.couple.fubble;

import static org.junit.Assert.assertEquals;

import java.util.List;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.junit.Assert;
import org.junit.Test;

import com.freckles.of.couple.fubble.helper.NumResponseCalculator;
import com.freckles.of.couple.fubble.proto.WebContainer.ChatMessage;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient.MessageTypeCase;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer;

public class ChatMessageTest extends WebsocketTest {

    private static final Logger LOGGER      = LogManager.getLogger(ChatMessageTest.class);

    private static final String ROOM_NAME_1 = "manu_test1";

    @Test
    public void testBroadcastChatMessage() {
        try {
            // 5. client joins
            List<FubbleClientEndpoint> clients = createClients(5);
            for (int index = 0; index < allClients.size(); index++) {
                FubbleClientEndpoint client = allClients.get(index);
                ResponseCount.startLatch(NumResponseCalculator.numJoinRoomResponses(index));
                joinRoom(ROOM_NAME_1, client);
                ResponseCount.await();
            }

            // Fubbler2 sends a chat message
            FubbleClientEndpoint fubbler2 = clients.stream().filter(client -> client.getUserName().equals("Fubbler2")).findFirst().get();
            String greetings = "Good morning everyone!";
            ChatMessage chatMessage = ChatMessage.newBuilder().setContent(greetings).build();
            MessageContainerServer message = MessageContainerServer.newBuilder().setChatMessage(chatMessage).build();
            ResponseCount.startLatch(NumResponseCalculator.numChatResponses(clients.size()));
            sendMessage(message, fubbler2);
            ResponseCount.await();

            // check if everyone received the message
            for (FubbleClientEndpoint client : clients) {
                MessageContainerClient chatMsg = client.getMessages().stream()//
                    .filter(msg -> msg.getMessageTypeCase().equals(MessageTypeCase.CHAT_MESSAGE)) //
                    .findFirst().get();
                assertEquals(greetings, chatMsg.getChatMessage().getContent());
            }

        } catch (Exception ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
            Assert.fail();
        }
    }

}
