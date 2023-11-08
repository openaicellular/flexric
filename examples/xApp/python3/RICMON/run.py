import argparse
import ricmon, preprocs


if __name__ == '__main__':
    # Quick-and-dirty handling of ODIN arguments
    parser = argparse.ArgumentParser(description='Paths to FlexApp and RICMON configs.')
    parser.add_argument('-c', '--configs', type=str, required=True,
                        help="Absolute path to FlexApp's X.conf")
    parser.add_argument('-m', '--mappings', type=str, required=True,
                        help="Absolute path to RICMON's Y.json")
    args = parser.parse_args()

    # Parse JSON configs supplied by ODIN
    running_configs, metrics_preprocs = ricmon.parse_configs(args.mappings, preprocs)

    # Just run!
    ricmon.execute(args.configs, running_configs, metrics_preprocs)
