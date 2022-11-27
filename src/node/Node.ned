package node;

simple Node
{
parameters:
    int id;
    int x;
    int y;
    int n_nodes;
    string algorithm;
    double threshold_ratio;
    @display("i=misc/node_vs,gold");
gates:
    inout port[];
    input wirelessIn @directIn;
}