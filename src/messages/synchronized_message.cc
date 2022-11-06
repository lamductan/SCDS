#include "messages/synchronized_message.h"

SynchronizedMessage::SynchronizedMessage(const char* name, short kind) : cMessage(name, kind) {}

SynchronizedMessage::SynchronizedMessage(const SynchronizedMessage& other) : cMessage((cMessage) other) {
    sender_id = other.sender_id;
    round_id = other.round_id;
    synchronized_message_type = other.synchronized_message_type;
}

SynchronizedMessage& SynchronizedMessage::operator=(const SynchronizedMessage& other) {
    cMessage::operator=(other);
    sender_id = other.sender_id;
    round_id = other.round_id;
    synchronized_message_type = other.synchronized_message_type;
    return *this;
}

int SynchronizedMessage::getSenderId() const { return sender_id; }

void SynchronizedMessage::setSenderId(int sender_id) {
    this->sender_id = sender_id;
}

int SynchronizedMessage::getRoundId() const { return round_id; }

void SynchronizedMessage::setRoundId(int round_id) {
    this->round_id = round_id;
}

SynchronizedMessageType SynchronizedMessage::getSynchronizedMessageType() const { return synchronized_message_type; }

void SynchronizedMessage::setSynchronizedMessageType(SynchronizedMessageType synchronized_message_type) {
    this->synchronized_message_type = synchronized_message_type;
}

SynchronizedMessage::~SynchronizedMessage() {}