package networks;

//
// Dynamically sets up a network, with the topology coming from text files
//
simple Network
{
    parameters:
        string networkFile = default("gen_random_networks/20/sparse/000.txt");
    gates:
        input wirelessIn @directIn;
}