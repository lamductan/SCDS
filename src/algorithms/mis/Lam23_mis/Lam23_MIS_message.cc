#include "algorithms/mis/Lam23_mis/Lam23_MIS_message.h"

LamMISMessage::LamMISMessage(const char* name, short kind) : LubyMISMessage(name, kind) {}

void LamMISMessage::copyInformation(LamTwoRSNodeStatus two_rs_status, int color) {
    this->two_rs_status = two_rs_status;
    this->color = color;
}

void LamMISMessage::copyInformation(
    int sender_id, bool marked, int degree, NodeStatus status, LamTwoRSNodeStatus two_rs_status, int color
) {
    LubyMISMessage::copyInformation(sender_id, marked, degree, status);
    copyInformation(two_rs_status, color);
}

LamMISMessage::LamMISMessage(const LamMISMessage& other) : LubyMISMessage((LubyMISMessage) other) {
    copyInformation(other.two_rs_status, other.color);
}

LamMISMessage& LamMISMessage::operator=(const LamMISMessage& other) {
    LamMISMessage::operator=(other);
    copyInformation(other.two_rs_status, other.color);
    return *this;
}

LamTwoRSNodeStatus LamMISMessage::getTwoRSStatus() const { return two_rs_status; }

void LamMISMessage::setTwoRSStatus(LamTwoRSNodeStatus two_rs_status) {
    this->two_rs_status = two_rs_status;
}

int LamMISMessage::getColor() const { return color; }

void LamMISMessage::setColor(int color) {
    this->color = color;
}

int LamMISMessage::getClusterCenterId() const { return cluster_center_id; }

void LamMISMessage::setClusterCenterId(int cluster_center_id) {
    this->cluster_center_id = cluster_center_id;
}