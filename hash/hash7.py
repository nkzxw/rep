# weight

# 引入weight的概念，目的是“能者多劳”：
# 解决未来添加存储能力更大的node时，使得可以分配到更多的partition。
# 例如，2T 容量的node的partition数为1T的两倍。
# 在ring的构建中，加入了weight属性。
# 本例中weight简单地取1和2两个值，根据每个结点在weight和中的比例分配所需partition数。


from array import array
from hashlib import md5
from random import shuffle
from struct import unpack_from
from time import time

class Ring(object):

    def __init__(self, nodes, part2node, replicas):
        self.nodes = nodes
        self.part2node = part2node
        self.replicas = replicas
        partition_power = 1
        while 2 ** partition_power < len(part2node):
            partition_power += 1
        if len(part2node) != 2 ** partition_power:
            raise Exception("part2node's length is not an "
                            "exact power of 2")
        self.partition_shift = 32 - partition_power

    def get_nodes(self, data_id):
        part = unpack_from('>I',
           md5(str(data_id).encode("utf-8")).digest())[0] >> self.partition_shift
        node_ids = [self.part2node[part]]
        zones = [self.nodes[node_ids[0]]]
        for replica in range(1, self.replicas):
            while self.part2node[part] in node_ids and \
                   self.nodes[self.part2node[part]] in zones:
                part += 1
                if part >= len(self.part2node):
                    part = 0
            node_ids.append(self.part2node[part])
            zones.append(self.nodes[node_ids[-1]])
        return [self.nodes[n] for n in node_ids]

def build_ring(nodes, partition_power, replicas):
    begin = time()
    parts = 2 ** partition_power
    total_weight = \
        float(sum(n['weight'] for n in nodes.values()))
    for node in nodes.values():
        node['desired_parts'] = \
            parts / total_weight * node['weight']
    part2node = array('H')
    for part in range(2 ** partition_power):
        for node in nodes.values():
            if node['desired_parts'] >= 1:
                node['desired_parts'] -= 1
                part2node.append(node['id'])
                break
        else:
            for node in nodes.values():
                if node['desired_parts'] >= 0:
                    node['desired_parts'] -= 1
                    part2node.append(node['id'])
                    break
    shuffle(part2node)
    ring = Ring(nodes, part2node, replicas)
    print('%.02fs to build ring' % (time() - begin))
    return ring

def test_ring(ring):
    begin = time()
    DATA_ID_COUNT = 10000000
    node_counts = {}
    zone_counts = {}
    for data_id in range(DATA_ID_COUNT):
        for node in ring.get_nodes(data_id):
            node_counts[node['id']] = \
                node_counts.get(node['id'], 0) + 1
            zone_counts[node['zone']] = \
                zone_counts.get(node['zone'], 0) + 1
    print('%ds to test ring' % (time() - begin))
    total_weight = float(sum(n['weight'] for n in
                             ring.nodes.values()))
    max_over = 0
    max_under = 0
    for node in ring.nodes.values():
        desired = DATA_ID_COUNT * REPLICAS * \
            node['weight'] / total_weight
        diff = node_counts[node['id']] - desired
        if diff > 0:
            over = 100.0 * diff / desired
            if over > max_over:
                max_over = over
        else:
            under = 100.0 * (-diff) / desired
            if under > max_under:
                max_under = under
    print('%.02f%% max node over' % max_over)
    print('%.02f%% max node under' % max_under)
    max_over = 0
    max_under = 0
    for zone in set(n['zone'] for n in
                    ring.nodes.values()):
        zone_weight = sum(n['weight'] for n in
            ring.nodes.values() if n['zone'] == zone)
        desired = DATA_ID_COUNT * REPLICAS * \
            zone_weight / total_weight
        diff = zone_counts[zone] - desired
        if diff > 0:
            over = 100.0 * diff / desired
            if over > max_over:
                max_over = over
        else:
            under = 100.0 * (-diff) / desired
            if under > max_under:
                max_under = under
    print('%.02f%% max zone over' % max_over)
    print('%.02f%% max zone under' % max_under)

if __name__ == '__main__':
    PARTITION_POWER = 16
    REPLICAS = 3
    NODE_COUNT = 256
    ZONE_COUNT = 16
    nodes = {}
    while len(nodes) < NODE_COUNT:
        zone = 0
        while zone < ZONE_COUNT and len(nodes) < NODE_COUNT:
            node_id = len(nodes)
            nodes[node_id] = {'id': node_id, 'zone': zone,
                              'weight': 1.0 + (node_id % 2)}
            zone += 1
    ring = build_ring(nodes, PARTITION_POWER, REPLICAS)
    test_ring(ring)
