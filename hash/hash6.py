# 随机分配映射
#
# 首先part2node是基于顺序分配的，对于给定的node，它所有partition的copies均在另两个node上，
# 若某个node频繁宕机，与它相应的两个node上的数据项需要频繁复制。
# 解决方法是随机分配partition到node的映射。

# 分区容忍性和引入zone
#
# 其次是当前的集群不满足CAP原理中的分区容忍性（Partition Tolerance）。Gilbert 和Lynch将分区容忍性定义如下：
# No set of failures less than total network failure is allowed to cause the system to respond incorrectly。
# 翻译一下，就是除了全部网络节点发生故障以外，所有子节点集合的故障都不允许导致整个系统的响应故障。
# 现在为止，这些node都在一个机架上，一旦发生断电，网络故障，那么将丧失这一性质。
# 因此就需要一种机制对机器的物理位置进行隔离。所以引入了zone的概念。
# 在ring代码中引入zone_count，把这些node分割到16个zone中去。
# 其中partition的replica不能放在同一个node上或同一个zone内。
# 
# 
# 到目前为止，ring的基本功能都已经有了：一致性哈希ring、partition、partition power、replica、zone。
# 目前还差weight以及将以上代码改写为类封装成module。

from array import array
from hashlib import md5
from random import shuffle
from struct import unpack_from

REPLICAS = 3
PARTITION_POWER = 16
PARTITION_SHIFT = 32 - PARTITION_POWER
PARTITION_MAX = 2 ** PARTITION_POWER - 1
NODE_COUNT = 256
ZONE_COUNT = 16
DATA_ID_COUNT = 10000000

node2zone = []
while len(node2zone) < NODE_COUNT:
    zone = 0
    while zone < ZONE_COUNT and len(node2zone) < NODE_COUNT:
        node2zone.append(zone)
        zone += 1
part2node = array('H')
for part in range(2 ** PARTITION_POWER):
    part2node.append(part % NODE_COUNT)
shuffle(part2node)
node_counts = [0] * NODE_COUNT
zone_counts = [0] * ZONE_COUNT
for data_id in range(DATA_ID_COUNT):
    part = unpack_from('>I',
        md5(str(data_id).encode("utf-8")).digest())[0] >> PARTITION_SHIFT
    node_ids = [part2node[part]]
    zones = [node2zone[node_ids[0]]]
    node_counts[node_ids[0]] += 1
    zone_counts[zones[0]] += 1
    for replica in range(1, REPLICAS):
        while part2node[part] in node_ids and \
                node2zone[part2node[part]] in zones:
            part += 1
            if part > PARTITION_MAX:
                part = 0
        node_ids.append(part2node[part])
        zones.append(node2zone[node_ids[-1]])
        node_counts[node_ids[-1]] += 1
        zone_counts[zones[-1]] += 1
desired_count = DATA_ID_COUNT / NODE_COUNT * REPLICAS
print('%d: Desired data ids per node' % desired_count)
max_count = max(node_counts)
over = 100.0 * (max_count - desired_count) / desired_count
print('%d: Most data ids on one node, %.02f%% over' % \
    (max_count, over))
min_count = min(node_counts)
under = 100.0 * (desired_count - min_count) / desired_count
print('%d: Least data ids on one node, %.02f%% under' % \
    (min_count, under))
desired_count = DATA_ID_COUNT / ZONE_COUNT * REPLICAS
print('%d: Desired data ids per zone' % desired_count)
max_count = max(zone_counts)
over = 100.0 * (max_count - desired_count) / desired_count
print('%d: Most data ids in one zone, %.02f%% over' % \
    (max_count, over))
min_count = min(zone_counts)
under = 100.0 * (desired_count - min_count) / desired_count
print('%d: Least data ids in one zone, %.02f%% under' % \
    (min_count, under))