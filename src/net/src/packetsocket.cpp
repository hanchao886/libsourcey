///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <http://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#include "scy/net/packetsocket.h"


using std::endl;


namespace scy {
namespace net {


//
// Packet Socket Adapter
//


PacketSocketEmitter::PacketSocketEmitter(const Socket::Ptr& socket)
    : SocketEmitter(socket) //reinterpret_cast<SocketAdapter*>(socket.get()).get()
    //, socket(socket)
{
    // TraceS(this) << "Create: " << socket << endl;
}


void PacketSocketEmitter::onSocketRecv(Socket& sock, const MutableBuffer& buffer, const Address& peerAddress)
{
    // TraceS(this) << "Recv: " << buffer.size() << endl;

    IPacket* pkt = nullptr;
    const char* buf = bufferCast<const char*>(buffer);
    std::size_t len = buffer.size();
    std::size_t nread = 0;
    while (len > 0 && (pkt = factory.createPacket(constBuffer(buf, len), nread))) {
        assert(nread > 0);
        pkt->info = new PacketInfo(this->impl, peerAddress);
        onPacket(*pkt);
        delete pkt;
        buf += nread;
        len -= nread;
    }
}


void PacketSocketEmitter::onPacket(IPacket& pkt)
{
    // TraceS(this) << "On packet: " << pkt.size() << endl;
    PacketSignal::emit(pkt);
}


#if 0
//
// Packet Socket
//


PacketSocket::PacketSocket(const Socket& socket) :
    Socket(socket)
{
    addReceiver(new PacketSocketEmitter);
    //assert(Socket::base().refCount() >= 2);
}


PacketSocket::PacketSocket(Socket* base, bool shared) :
    Socket(base, shared)
{
    addReceiver(new PacketSocketEmitter);
    //assert(!shared || Socket::base().refCount() >= 2);
}


PacketSocket::~PacketSocket()
{
}


//
// Packet Stream Socket Adapter
//


PacketStreamSocketAdapter::PacketStreamSocketAdapter(Socket& socket) :
    PacketProcessor(PacketStreamSocketAdapter::emitter),
    _socket(socket)
{
}


PacketStreamSocketAdapter::~PacketStreamSocketAdapter()
{
}


void PacketStreamSocketAdapter::process(IPacket& packet)
{
    // TraceS(this) << "Process: " << packet.className() << endl;

    //Mutex::ScopedLock lock(_mutex);

    // TODO: Split packet if needed
    _socket.send(packet);
}


bool PacketStreamSocketAdapter::accepts(IPacket* packet)
{
    return dynamic_cast<RawPacket*>(&packet) != 0;
}


void PacketStreamSocketAdapter::onStreamStateChange(const PacketStreamState& state)
{
    // TraceS(this) << "Stream state change: " << state << endl;

    // TODO: Sync socket with stream?

    //Mutex::ScopedLock lock(_mutex);

    switch (state.id()) {
    case PacketStreamState::Running:
        break;

    case PacketStreamState::Stopped:
    case PacketStreamState::Error:
    case PacketStreamState::Resetting:
        break;
    //case PacketStreamState::None:
    //case PacketStreamState::Stopping:
    //case PacketStreamState::Closed:
    }
}
#endif


} // namespace net
} // namespace scy


/// @\}
