import ricmon
import inspect, preprocs


if __name__ == '__main__':
    # ODIN: load JSON configs
    configs_path = "/home/acilius/ricmon/conf_files/ricmon.json"
    running_configs, preproc_configs = ricmon.parse_configs(configs_path)

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
    ricmon.execute(running_configs, metrics_to_fns)