#!/usr/bin/python

import os
import sys
import yaml
from collections import defaultdict, Counter
from pprint import pprint
import argparse
import subprocess

parser = argparse.ArgumentParser(description='Prepare yaml for esphome')
parser.add_argument('--deps', action=argparse.BooleanOptionalAction)
parser.add_argument('filename', type=str)

args = parser.parse_args()

deps = []

class Object:
    def __init__(self, value):
        self.value = value


def object_repr(dumper, value):
    tag = value.value.tag
    val = value.value.value
    style = value.value.style

    return value.value #dumper.represent_scalar(tag=tag, value=val, style=style)

#dumper.represent_mapping(lmbd.tag, lmbd._txt)

def object_ctor(loader, node):
    return Object(node)

def secret_ctor(loader, node):
    return "!secret " + node.value

def join(k, v, data):
    if k in data and data[k]:
        if type(data[k]) != type(v):
            raise Exception(f"mixed types can't be merged {data[k]} != {v}")
        if isinstance(data[k], dict):
            data[k].update(v)
        else:
            data[k] += v
    else:
        data[k] = v

def parse_preserving_duplicates(input_file):
    class PreserveDuplicatesLoader(yaml.CLoader):
        pass

    def map_constructor(loader, node, deep=False):
        """Walk tree, removing degeneracy in any duplicate keys"""
        data = {}

        for key, val in node.value:
            k = loader.construct_object(key, deep=deep)
            v = loader.construct_object(val, deep=True)

            join(k, v, data)

        return data

    PreserveDuplicatesLoader.add_constructor(yaml.resolver.BaseResolver.DEFAULT_MAPPING_TAG,
                                             map_constructor)

    loader = PreserveDuplicatesLoader
    loader.add_constructor("!lambda", object_ctor)
    loader.add_constructor("!secret", object_ctor)

    return yaml.load(input_file, loader)

def join_load(dirname, filename):
    dirname = os.path.join(dirname, os.path.dirname(filename))
    filename = os.path.basename(filename)
    path = os.path.join(dirname, filename)

    with open(path, 'r') as f:
        y = parse_preserving_duplicates(f)

        if "packages" in y:
            for package in y["packages"]:
                global deps
                deps += [ os.path.join(dirname, package) ]

                yp = join_load(dirname, package)
                for k,v in yp.items():
                    join(k, v, y)
            y.pop("packages")

        return y

filename = os.path.basename(sys.argv[1])
dirname = os.path.dirname(sys.argv[1])

data = join_load(dirname, filename)

if args.deps:
    print(f"{sys.argv[1]}: " + "\\\n   ".join(deps))
    sys.exit(0)

version = subprocess.check_output("echo `hostname` `git describe --all --long  --dirty` `date --rfc-email`", shell=True, text=True).strip()

autosensors = parse_preserving_duplicates("""
text_sensor:
  - platform: template
    name: ${device} Version
    lambda: !lambda return {"%s"};

  - platform: template
    name: ${device} Filename
    lambda: !lambda return {"%s"};
""" % (version, filename))

for k,v in autosensors.items():
    join(k, v, data)

dumper = yaml.Dumper
dumper.add_representer(Object, object_repr)

print(yaml.dump(data, Dumper=dumper))
