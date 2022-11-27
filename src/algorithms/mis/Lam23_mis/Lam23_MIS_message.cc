#include "algorithms/mis/Lam23_mis/Lam23_MIS_message.h"

LamMISMessage::LamMISMessage(const char *name, short kind)
    : LubyMISMessage(name, kind)
{
}

void LamMISMessage::copyInformation(LamTwoRSNodeStatus two_rs_status, int color,
                                    int two_rs_cluster_center_id)
{
    this->two_rs_status = two_rs_status;
    this->color = color;
    this->two_rs_cluster_center_id = two_rs_cluster_center_id;
}

void LamMISMessage::copyInformation(int sender_id, bool marked, int degree,
                                    MISNodeStatus MIS_status,
                                    LamTwoRSNodeStatus two_rs_status, int color,
                                    int two_rs_cluster_center_id)
{
    LubyMISMessage::copyInformation(sender_id, marked, degree, MIS_status);
    copyInformation(two_rs_status, color, two_rs_cluster_center_id);
}

LamMISMessage::LamMISMessage(const LamMISMessage &other)
    : LubyMISMessage((LubyMISMessage)other)
{
    copyInformation(other.two_rs_status, other.color,
                    other.two_rs_cluster_center_id);
}

LamMISMessage &LamMISMessage::operator=(const LamMISMessage &other)
{
    LamMISMessage::operator=(other);
    copyInformation(other.two_rs_status, other.color,
                    other.two_rs_cluster_center_id);
    return *this;
}

LamTwoRSNodeStatus LamMISMessage::getTwoRSStatus() const
{
    return two_rs_status;
}

void LamMISMessage::setTwoRSStatus(LamTwoRSNodeStatus two_rs_status)
{
    this->two_rs_status = two_rs_status;
}

int LamMISMessage::getColor() const { return color; }

void LamMISMessage::setColor(int color) { this->color = color; }

int LamMISMessage::getClusterCenterId() const
{
    return two_rs_cluster_center_id;
}

void LamMISMessage::setClusterCenterId(int two_rs_cluster_center_id)
{
    this->two_rs_cluster_center_id = two_rs_cluster_center_id;
}