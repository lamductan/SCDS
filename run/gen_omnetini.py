# Usage:
# python gen_network.py

import os
import argparse
import tqdm

parser = argparse.ArgumentParser()
parser.add_argument('--t', type=int, help='number of networks', default=50)
args = parser.parse_args()


def gen_omnetini_and_save_file(network_id, n, denseness):
    file_path = os.path.join(str(n), denseness, 'omnetpp_{:03d}.ini'.format(network_id))
    os.makedirs(os.path.dirname(file_path), exist_ok=True)
    with open(file_path, "w") as f:
        f.write("include ../../omnetpp_template.ini\n")
        f.write("\n")
        f.write("[General]\n")
        f.write(f'**.networkFile = \"gen_random_networks/{n}/{denseness}/{network_id:03d}.txt\"')
        f.close()


def gen(n, denseness):
    t = args.t

    for network_id in range(t):
        gen_omnetini_and_save_file(network_id, n, denseness)

if __name__ == '__main__':
    for n in tqdm.tqdm([20, 50, 100] + list(range(200, 2001, 100))):
        print("n =", n)
        for denseness in ['sparse', 'medium', 'dense']:
            print(denseness)
            gen(n, denseness)
