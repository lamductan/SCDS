#include "algorithms/mis/Luby_mis/Luby_MIS_message.h"

LubyMISMessage::LubyMISMessage(const char* name, short kind) : cMessage(name, kind) {}

void LubyMISMessage::copyInformation(int sender_id, bool marked, int degree, NodeStatus status) {
    this->sender_id = sender_id;
    this->marked = marked;
    this->degree = degree;
    this->status = status;
}

LubyMISMessage::LubyMISMessage(const LubyMISMessage& other) : cMessage((cMessage) other) {
    copyInformation(other.sender_id, other.marked, other.degree, other.status);
}

LubyMISMessage& LubyMISMessage::operator=(const LubyMISMessage& other) {
    cMessage::operator=(other);
    copyInformation(other.sender_id, other.marked, other.degree, other.status);
    return *this;
}

int LubyMISMessage::getSenderId() const { return sender_id; }

void LubyMISMessage::setSenderId(int sender_id) {
    this->sender_id = sender_id;
}

bool LubyMISMessage::getMarked() const {
    return marked;
}

void LubyMISMessage::setMarked(bool marked) {
    this->marked = marked;
}

int LubyMISMessage::getDegree() const {
    return degree;
}

void LubyMISMessage::setDegree(int degree) {
    this->degree = degree;
}

NodeStatus LubyMISMessage::getStatus() const {
    return status;
}

void LubyMISMessage::setStatus(NodeStatus status) {
    this->status = status;
}

LubyMISMessage::~LubyMISMessage() {}
