package node;

simple Node
{
    parameters:
        int id;
        int x;
        int y;
        @display("i=misc/node_vs,gold");
    gates:
        inout port[];
        input wirelessIn @directIn;
}