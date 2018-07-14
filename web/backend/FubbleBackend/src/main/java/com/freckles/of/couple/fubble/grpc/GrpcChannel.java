
package com.freckles.of.couple.fubble.grpc;

import com.freckles.of.couple.fubble.grpc.RtcGrpc.RtcBlockingStub;
import com.freckles.of.couple.fubble.tools.FubbleProperties;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;

public class GrpcChannel {

    private static GrpcChannel   instance;
    private final ManagedChannel rtcChannel;

    private GrpcChannel() {
        String rtcServerHost = FubbleProperties.getInstance().getRtcServerHost();
        int rtcServerPort = FubbleProperties.getInstance().getRtcServerPort();
        ManagedChannelBuilder<?> channelBuilder = ManagedChannelBuilder.forAddress(rtcServerHost, rtcServerPort).usePlaintext();
        rtcChannel = channelBuilder.build();
    }

    public static GrpcChannel getInstance() {
        if (GrpcChannel.instance == null) {
            GrpcChannel.instance = new GrpcChannel();
        }

        return GrpcChannel.instance;
    }

    public ManagedChannel getRtcChannel() {
        return rtcChannel;
    }

    public RtcBlockingStub createBlockingStub() {
        return RtcGrpc.newBlockingStub(rtcChannel);
    }

}
