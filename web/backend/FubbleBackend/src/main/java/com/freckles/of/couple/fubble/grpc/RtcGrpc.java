
package com.freckles.of.couple.fubble.grpc;

import static io.grpc.MethodDescriptor.generateFullMethodName;
import static io.grpc.stub.ClientCalls.asyncUnaryCall;
import static io.grpc.stub.ClientCalls.blockingUnaryCall;
import static io.grpc.stub.ClientCalls.futureUnaryCall;
import static io.grpc.stub.ServerCalls.asyncUnaryCall;
import static io.grpc.stub.ServerCalls.asyncUnimplementedUnaryCall;

/**
 */
@javax.annotation.Generated(value = "by gRPC proto compiler (version 1.13.2)", comments = "Source: web_rtc.proto")
public final class RtcGrpc {

    private RtcGrpc() {}

    public static final String                                                                                                                                                                       SERVICE_NAME = "server.Rtc";

    // Static method descriptors that strictly reflect the proto.
    private static volatile io.grpc.MethodDescriptor<com.freckles.of.couple.fubble.grpc.RtcMessageContainer.JoinRoomRequest, com.freckles.of.couple.fubble.grpc.RtcMessageContainer.RoomInformation> getJoinRoomMethod;

    public static io.grpc.MethodDescriptor<com.freckles.of.couple.fubble.grpc.RtcMessageContainer.JoinRoomRequest, com.freckles.of.couple.fubble.grpc.RtcMessageContainer.RoomInformation> getJoinRoomMethod() {
        io.grpc.MethodDescriptor<com.freckles.of.couple.fubble.grpc.RtcMessageContainer.JoinRoomRequest, com.freckles.of.couple.fubble.grpc.RtcMessageContainer.RoomInformation> getJoinRoomMethod;
        if ((getJoinRoomMethod = RtcGrpc.getJoinRoomMethod) == null) {
            synchronized (RtcGrpc.class) {
                if ((getJoinRoomMethod = RtcGrpc.getJoinRoomMethod) == null) {
                    RtcGrpc.getJoinRoomMethod = getJoinRoomMethod = io.grpc.MethodDescriptor.<com.freckles.of.couple.fubble.grpc.RtcMessageContainer.JoinRoomRequest, com.freckles.of.couple.fubble.grpc.RtcMessageContainer.RoomInformation> newBuilder().setType(
                        io.grpc.MethodDescriptor.MethodType.UNARY).setFullMethodName(
                            generateFullMethodName("server.Rtc", "JoinRoom")).setSampledToLocalTracing(true).setRequestMarshaller(
                                io.grpc.protobuf.ProtoUtils.marshaller(
                                    com.freckles.of.couple.fubble.grpc.RtcMessageContainer.JoinRoomRequest.getDefaultInstance())).setResponseMarshaller(
                                        io.grpc.protobuf.ProtoUtils.marshaller(
                                            com.freckles.of.couple.fubble.grpc.RtcMessageContainer.RoomInformation.getDefaultInstance())).setSchemaDescriptor(
                                                new RtcMethodDescriptorSupplier("JoinRoom")).build();
                }
            }
        }
        return getJoinRoomMethod;
    }

    private static volatile io.grpc.MethodDescriptor<com.freckles.of.couple.fubble.grpc.RtcMessageContainer.DeleteRoomRequest, com.google.protobuf.Empty> getDeleteRoomMethod;

    public static io.grpc.MethodDescriptor<com.freckles.of.couple.fubble.grpc.RtcMessageContainer.DeleteRoomRequest, com.google.protobuf.Empty> getDeleteRoomMethod() {
        io.grpc.MethodDescriptor<com.freckles.of.couple.fubble.grpc.RtcMessageContainer.DeleteRoomRequest, com.google.protobuf.Empty> getDeleteRoomMethod;
        if ((getDeleteRoomMethod = RtcGrpc.getDeleteRoomMethod) == null) {
            synchronized (RtcGrpc.class) {
                if ((getDeleteRoomMethod = RtcGrpc.getDeleteRoomMethod) == null) {
                    RtcGrpc.getDeleteRoomMethod = getDeleteRoomMethod = io.grpc.MethodDescriptor.<com.freckles.of.couple.fubble.grpc.RtcMessageContainer.DeleteRoomRequest, com.google.protobuf.Empty> newBuilder().setType(
                        io.grpc.MethodDescriptor.MethodType.UNARY).setFullMethodName(
                            generateFullMethodName("server.Rtc", "DeleteRoom")).setSampledToLocalTracing(true).setRequestMarshaller(
                                io.grpc.protobuf.ProtoUtils.marshaller(
                                    com.freckles.of.couple.fubble.grpc.RtcMessageContainer.DeleteRoomRequest.getDefaultInstance())).setResponseMarshaller(
                                        io.grpc.protobuf.ProtoUtils.marshaller(com.google.protobuf.Empty.getDefaultInstance())).setSchemaDescriptor(
                                            new RtcMethodDescriptorSupplier("DeleteRoom")).build();
                }
            }
        }
        return getDeleteRoomMethod;
    }

    /**
     * Creates a new async stub that supports all call types for the service
     */
    public static RtcStub newStub(io.grpc.Channel channel) {
        return new RtcStub(channel);
    }

    /**
     * Creates a new blocking-style stub that supports unary and streaming output calls on the service
     */
    public static RtcBlockingStub newBlockingStub(io.grpc.Channel channel) {
        return new RtcBlockingStub(channel);
    }

    /**
     * Creates a new ListenableFuture-style stub that supports unary calls on the service
     */
    public static RtcFutureStub newFutureStub(io.grpc.Channel channel) {
        return new RtcFutureStub(channel);
    }

    /**
     */
    public static abstract class RtcImplBase implements io.grpc.BindableService {

        /**
         */
        public void joinRoom(com.freckles.of.couple.fubble.grpc.RtcMessageContainer.JoinRoomRequest request,
            io.grpc.stub.StreamObserver<com.freckles.of.couple.fubble.grpc.RtcMessageContainer.RoomInformation> responseObserver) {
            asyncUnimplementedUnaryCall(getJoinRoomMethod(), responseObserver);
        }

        /**
         */
        public void deleteRoom(com.freckles.of.couple.fubble.grpc.RtcMessageContainer.DeleteRoomRequest request,
            io.grpc.stub.StreamObserver<com.google.protobuf.Empty> responseObserver) {
            asyncUnimplementedUnaryCall(getDeleteRoomMethod(), responseObserver);
        }

        @java.lang.Override
        public final io.grpc.ServerServiceDefinition bindService() {
            return io.grpc.ServerServiceDefinition.builder(getServiceDescriptor()).addMethod(getJoinRoomMethod(), asyncUnaryCall(
                new MethodHandlers<com.freckles.of.couple.fubble.grpc.RtcMessageContainer.JoinRoomRequest, com.freckles.of.couple.fubble.grpc.RtcMessageContainer.RoomInformation>(
                    this, METHODID_JOIN_ROOM))).addMethod(
                        getDeleteRoomMethod(),
                        asyncUnaryCall(
                            new MethodHandlers<com.freckles.of.couple.fubble.grpc.RtcMessageContainer.DeleteRoomRequest, com.google.protobuf.Empty>(
                                this, METHODID_DELETE_ROOM))).build();
        }
    }

    /**
     */
    public static final class RtcStub extends io.grpc.stub.AbstractStub<RtcStub> {
        private RtcStub(io.grpc.Channel channel) {
            super(channel);
        }

        private RtcStub(io.grpc.Channel channel, io.grpc.CallOptions callOptions) {
            super(channel, callOptions);
        }

        @java.lang.Override
        protected RtcStub build(io.grpc.Channel channel, io.grpc.CallOptions callOptions) {
            return new RtcStub(channel, callOptions);
        }

        /**
         */
        public void joinRoom(com.freckles.of.couple.fubble.grpc.RtcMessageContainer.JoinRoomRequest request,
            io.grpc.stub.StreamObserver<com.freckles.of.couple.fubble.grpc.RtcMessageContainer.RoomInformation> responseObserver) {
            asyncUnaryCall(getChannel().newCall(getJoinRoomMethod(), getCallOptions()), request, responseObserver);
        }

        /**
         */
        public void deleteRoom(com.freckles.of.couple.fubble.grpc.RtcMessageContainer.DeleteRoomRequest request,
            io.grpc.stub.StreamObserver<com.google.protobuf.Empty> responseObserver) {
            asyncUnaryCall(getChannel().newCall(getDeleteRoomMethod(), getCallOptions()), request, responseObserver);
        }
    }

    /**
     */
    public static final class RtcBlockingStub extends io.grpc.stub.AbstractStub<RtcBlockingStub> {
        private RtcBlockingStub(io.grpc.Channel channel) {
            super(channel);
        }

        private RtcBlockingStub(io.grpc.Channel channel, io.grpc.CallOptions callOptions) {
            super(channel, callOptions);
        }

        @java.lang.Override
        protected RtcBlockingStub build(io.grpc.Channel channel, io.grpc.CallOptions callOptions) {
            return new RtcBlockingStub(channel, callOptions);
        }

        /**
         */
        public com.freckles.of.couple.fubble.grpc.RtcMessageContainer.RoomInformation joinRoom(
            com.freckles.of.couple.fubble.grpc.RtcMessageContainer.JoinRoomRequest request) {
            return blockingUnaryCall(getChannel(), getJoinRoomMethod(), getCallOptions(), request);
        }

        /**
         */
        public com.google.protobuf.Empty deleteRoom(com.freckles.of.couple.fubble.grpc.RtcMessageContainer.DeleteRoomRequest request) {
            return blockingUnaryCall(getChannel(), getDeleteRoomMethod(), getCallOptions(), request);
        }
    }

    /**
     */
    public static final class RtcFutureStub extends io.grpc.stub.AbstractStub<RtcFutureStub> {
        private RtcFutureStub(io.grpc.Channel channel) {
            super(channel);
        }

        private RtcFutureStub(io.grpc.Channel channel, io.grpc.CallOptions callOptions) {
            super(channel, callOptions);
        }

        @java.lang.Override
        protected RtcFutureStub build(io.grpc.Channel channel, io.grpc.CallOptions callOptions) {
            return new RtcFutureStub(channel, callOptions);
        }

        /**
         */
        public com.google.common.util.concurrent.ListenableFuture<com.freckles.of.couple.fubble.grpc.RtcMessageContainer.RoomInformation> joinRoom(
            com.freckles.of.couple.fubble.grpc.RtcMessageContainer.JoinRoomRequest request) {
            return futureUnaryCall(getChannel().newCall(getJoinRoomMethod(), getCallOptions()), request);
        }

        /**
         */
        public com.google.common.util.concurrent.ListenableFuture<com.google.protobuf.Empty> deleteRoom(
            com.freckles.of.couple.fubble.grpc.RtcMessageContainer.DeleteRoomRequest request) {
            return futureUnaryCall(getChannel().newCall(getDeleteRoomMethod(), getCallOptions()), request);
        }
    }

    private static final int METHODID_JOIN_ROOM   = 0;
    private static final int METHODID_DELETE_ROOM = 1;

    private static final class MethodHandlers<Req, Resp>
        implements io.grpc.stub.ServerCalls.UnaryMethod<Req, Resp>, io.grpc.stub.ServerCalls.ServerStreamingMethod<Req, Resp>,
        io.grpc.stub.ServerCalls.ClientStreamingMethod<Req, Resp>, io.grpc.stub.ServerCalls.BidiStreamingMethod<Req, Resp> {
        private final RtcImplBase serviceImpl;
        private final int         methodId;

        MethodHandlers(RtcImplBase serviceImpl, int methodId) {
            this.serviceImpl = serviceImpl;
            this.methodId = methodId;
        }

        @java.lang.Override
        @java.lang.SuppressWarnings("unchecked")
        public void invoke(Req request, io.grpc.stub.StreamObserver<Resp> responseObserver) {
            switch (methodId) {
                case METHODID_JOIN_ROOM:
                    serviceImpl.joinRoom((com.freckles.of.couple.fubble.grpc.RtcMessageContainer.JoinRoomRequest) request,
                        (io.grpc.stub.StreamObserver<com.freckles.of.couple.fubble.grpc.RtcMessageContainer.RoomInformation>) responseObserver);
                    break;
                case METHODID_DELETE_ROOM:
                    serviceImpl.deleteRoom((com.freckles.of.couple.fubble.grpc.RtcMessageContainer.DeleteRoomRequest) request,
                        (io.grpc.stub.StreamObserver<com.google.protobuf.Empty>) responseObserver);
                    break;
                default:
                    throw new AssertionError();
            }
        }

        @java.lang.Override
        @java.lang.SuppressWarnings("unchecked")
        public io.grpc.stub.StreamObserver<Req> invoke(io.grpc.stub.StreamObserver<Resp> responseObserver) {
            switch (methodId) {
                default:
                    throw new AssertionError();
            }
        }
    }

    private static abstract class RtcBaseDescriptorSupplier
        implements io.grpc.protobuf.ProtoFileDescriptorSupplier, io.grpc.protobuf.ProtoServiceDescriptorSupplier {
        RtcBaseDescriptorSupplier() {}

        @java.lang.Override
        public com.google.protobuf.Descriptors.FileDescriptor getFileDescriptor() {
            return com.freckles.of.couple.fubble.grpc.RtcMessageContainer.getDescriptor();
        }

        @java.lang.Override
        public com.google.protobuf.Descriptors.ServiceDescriptor getServiceDescriptor() {
            return getFileDescriptor().findServiceByName("Rtc");
        }
    }

    private static final class RtcFileDescriptorSupplier extends RtcBaseDescriptorSupplier {
        RtcFileDescriptorSupplier() {}
    }

    private static final class RtcMethodDescriptorSupplier extends RtcBaseDescriptorSupplier
        implements io.grpc.protobuf.ProtoMethodDescriptorSupplier {
        private final String methodName;

        RtcMethodDescriptorSupplier(String methodName) {
            this.methodName = methodName;
        }

        @java.lang.Override
        public com.google.protobuf.Descriptors.MethodDescriptor getMethodDescriptor() {
            return getServiceDescriptor().findMethodByName(methodName);
        }
    }

    private static volatile io.grpc.ServiceDescriptor serviceDescriptor;

    public static io.grpc.ServiceDescriptor getServiceDescriptor() {
        io.grpc.ServiceDescriptor result = serviceDescriptor;
        if (result == null) {
            synchronized (RtcGrpc.class) {
                result = serviceDescriptor;
                if (result == null) {
                    serviceDescriptor = result = io.grpc.ServiceDescriptor.newBuilder(SERVICE_NAME).setSchemaDescriptor(
                        new RtcFileDescriptorSupplier()).addMethod(getJoinRoomMethod()).addMethod(getDeleteRoomMethod()).build();
                }
            }
        }
        return result;
    }
}
