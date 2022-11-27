#include "algorithms/cds/cds_simple/simple_cds_message.h"

SimpleCDSMessage::SimpleCDSMessage(const char *name, short kind)
    : cMessage(name, kind)
{
}

void SimpleCDSMessage::copyInformation(
    int sender_id, const std::map<int, std::array<int, 3>> &MIS_info,
    const std::set<int> &cds_nodes)
{
    this->sender_id = sender_id;
    this->MIS_info = MIS_info;
    this->cds_nodes = cds_nodes;
}

SimpleCDSMessage::SimpleCDSMessage(const SimpleCDSMessage &other)
    : cMessage((cMessage)other)
{
    copyInformation(other.sender_id, other.MIS_info, other.cds_nodes);
}

SimpleCDSMessage &SimpleCDSMessage::operator=(const SimpleCDSMessage &other)
{
    cMessage::operator=(other);
    copyInformation(other.sender_id, other.MIS_info, other.cds_nodes);
    return *this;
}

int SimpleCDSMessage::getSenderId() const { return sender_id; }

void SimpleCDSMessage::setSenderId(int sender_id)
{
    this->sender_id = sender_id;
}

std::map<int, std::array<int, 3>> SimpleCDSMessage::getMISInfo() const
{
    return MIS_info;
}

void SimpleCDSMessage::setMISInfo(
    const std::map<int, std::array<int, 3>> &MIS_info)
{
    this->MIS_info = MIS_info;
}

std::set<int> SimpleCDSMessage::getCDSNodes() const { return cds_nodes; }

void SimpleCDSMessage::setCDSNodes(const std::set<int> &cds_nodes)
{
    this->cds_nodes = cds_nodes;
}

std::string SimpleCDSMessage::to_string(int log_level) const
{
    std::string one_tab = std::string(4, ' ');
    std::string tab = std::string(log_level * 4, ' ');
    std::string s = tab + "{\n";
    s += (tab + one_tab + "sender_id = " + std::to_string(sender_id) + ",\n");
    s += (tab + one_tab + "MIS_info = [\n");
    for (auto it : MIS_info) {
        s += tab + one_tab + one_tab + std::to_string(it.first) + ": (";
        for (int x : it.second)
            s += std::to_string(x) + ",";
        s += ")\n";
    }
    s += (tab + one_tab + "],\n");
    s += (tab + one_tab + "CDS_nodes = [");
    for (int x : cds_nodes)
        s += std::to_string(x) + ",";
    s += "]\n";
    return (s += tab + "}");
}