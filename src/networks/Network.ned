package networks;

//
// Dynamically sets up a network, with the topology coming from text files
//
simple Network
{
    parameters:
        string networkFile = default("gen_random_networks/20/sparse/001.txt");
        string algorithm = default("MIS-Luby");
    gates:
        input wirelessIn @directIn;
}