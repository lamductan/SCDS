#include "algorithms/mis/KW08_mis/KW08_MIS_message.h"

KW08MISMessage::KW08MISMessage(const char* name, short kind) : cMessage(name, kind) {}

void KW08MISMessage::copyInformation(int sender_id, int r, KW08NodeStatus status) {
    this->sender_id = sender_id;
    this->r = r;
    this->status = status;
}

KW08MISMessage::KW08MISMessage(const KW08MISMessage& other) : cMessage((cMessage) other) {
    copyInformation(other.sender_id, other.r, other.status);
}

KW08MISMessage& KW08MISMessage::operator=(const KW08MISMessage& other) {
    cMessage::operator=(other);
    copyInformation(other.sender_id, other.r, other.status);
    return *this;
}

int KW08MISMessage::getSenderId() const { return sender_id; }

void KW08MISMessage::setSenderId(int sender_id) {
    this->sender_id = sender_id;
}

int KW08MISMessage::getR() const {
    return r;
}

void KW08MISMessage::setR(int r) {
    this->r = r;
}

KW08NodeStatus KW08MISMessage::getStatus() const {
    return status;
}

void KW08MISMessage::setStatus(KW08NodeStatus status) {
    this->status = status;
}

KW08MISMessage::~KW08MISMessage() {}
