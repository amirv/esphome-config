#!/usr/bin/python

import sys
import yaml
from yaml.nodes import *
from yaml.error import *
from yaml.reader import *
from yaml.scanner import *
from yaml.parser import *
from yaml.composer import *
from yaml.resolver import *
from collections import defaultdict, Counter

class MySafeConstructor(yaml.constructor.SafeConstructor):
    def flatten_mapping(self, node):
            merge = []
            index = 0
            while index < len(node.value):
                key_node, value_node = node.value[index]
                if key_node.tag == 'tag:yaml.org,2002:merge':
                    del node.value[index]
                    if isinstance(value_node, ScalarNode) and value_node.tag == '!include':
                        print(f"ZZZZZZZZZZZ {value_node.value}")
                        with open(value_node.value, 'rb') as fp:
                            inc = yaml.compose(fp, Loader)
                            print(f"ZZZZZZZZZZZ {inc}")
                            merge.extend(inc.value)
                    elif isinstance(value_node, MappingNode):
                        self.flatten_mapping(value_node)
                        merge.extend(value_node.value)
                    elif isinstance(value_node, SequenceNode):
                        submerge = []
                        for subnode in value_node.value:
                            if not isinstance(subnode, MappingNode):
                                raise ConstructorError("while constructing a mapping",
                                                       node.start_mark,
                                                       "expected a mapping for merging, but found %s"
                                                       % subnode.id, subnode.start_mark)
                            self.flatten_mapping(subnode)
                            submerge.append(subnode.value)
                        submerge.reverse()
                        for value in submerge:
                            merge.extend(value)
                    else:
                        raise ConstructorError("while constructing a mapping", node.start_mark,
                                       "expected a mapping or list of mappings for merging, but found %s"
                                       % value_node.id, value_node.start_mark)
                elif key_node.tag == 'tag:yaml.org,2002:value':
                    key_node.tag = 'tag:yaml.org,2002:str'
                    index += 1
                else:
                    index += 1
            if bool(merge):
                node.merge = merge  # separate merge keys to be able to update without duplicate
                node.value = merge + node.value


class Loader(Reader, Scanner, Parser, Composer, MySafeConstructor, Resolver):

    def __init__(self, stream):
        Reader.__init__(self, stream)
        Scanner.__init__(self)
        Parser.__init__(self)
        Composer.__init__(self)
        MySafeConstructor.__init__(self)
        Resolver.__init__(self)

        yaml.add_constructor('!include', Loader.constructor_include, Loader)

    @staticmethod
    def constructor_include(loader: yaml.Loader, node: yaml.Node):
        print(f"value: {node.value} --- loader: {loader}")
        with open(node.value, 'rb') as _f:
            return yaml.load(_f, Loader)


if __name__ == '__main__':
    filename = sys.argv[1]

    with open(filename, 'r') as f:
        data = yaml.load(f, Loader)
    print(yaml.dump(data))
