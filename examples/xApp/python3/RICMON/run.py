import argparse
import ricmon, inspect, preprocs


if __name__ == '__main__':
    # Quick-and-dirty handling of ODIN arguments
    parser = argparse.ArgumentParser(description='Paths to FlexApp and RICMON configs.')
    parser.add_argument('-c', '--configs', type=str, required=True, 
                        help='Absolute path to FlexApp: X.conf')
    parser.add_argument('-m', '--mappings', type=str, required=True, 
                        help='Absolute path to RICMON: Y.json')
    args = parser.parse_args()

    # ODIN: load JSON configs
    running_configs, preproc_configs = ricmon.parse_configs(args.mappings)

    # ODIN: NOT YET work for external (absolute-path-ed) preprocs.py
    # => TODO: must be explicitly imported?!
    preproc_fns = {
        name: getattr(preprocs, name) 
        for name, obj in inspect.getmembers(preprocs)
        if (inspect.isfunction(obj) and name in dir(preprocs))
    }
    metrics_to_fns = {
        metric: [preproc_fns[pp_str] for pp_str in preproc_strs]
        for metric, preproc_strs in preproc_configs.items()
    }

    # Just run!
    ricmon.execute(args.configs, running_configs, metrics_to_fns)
