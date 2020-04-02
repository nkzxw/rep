# 保证数据安全，引入replica


# 到目前为止，在集群中的数据在本地节点上只有一份，
# 节点一旦发生故障就可能会造成数据的永久性丢失。
# 因此，Swift中引入replica的概念使用冗余副本来保证数据的安全。
# replica的默认值为3，其理论依据主要来源于NWR策略。
# NWR是一种在分布式存储系统中用于控制一致性级别的一种策略。
# 在Amazon的Dynamo云存储系统中，就应用NWR来控制一致性。每个字母的涵义如下：
# N：同一份数据的Replica的份数
# W：是更新一个数据对象的时候需要确保成功更新的份数
# R：读取一个数据需要读取的Replica的份数
# 在分布式系统中，数据的单点是不允许存在的。
# 即线上正常存在的Replica数量是1的情况是非常危险的，因为一旦这个Replica再次错误，就可能发生数据的永久性错误。
# 假如我们把N设置成为2，那么，只要有一个存储节点发生损坏，就会有单点的存在。所以N必须大于2。
# N越高，系统的维护和整体成本就越高。工业界通常把N设置为3。
# 因此，在ring的代码中引入replica，数量设置为3，
# 其中 node_ids记录的是3个replica存放的node id。
# part2node[part]是根据partition id 找到对应的node id。

# 结果如上，由于使用了256个node，分布约有1%的波动，比较均匀了。


from array import array
from hashlib import md5
from struct import unpack_from

REPLICAS = 3
PARTITION_POWER = 16
PARTITION_SHIFT = 32 - PARTITION_POWER
PARTITION_MAX = 2 ** PARTITION_POWER - 1
NODE_COUNT = 256
DATA_ID_COUNT = 10000000

part2node = array('H')
for part in range(2 ** PARTITION_POWER):
    part2node.append(part % NODE_COUNT)
node_counts = [0] * NODE_COUNT
for data_id in range(DATA_ID_COUNT):
    part = unpack_from('>I',
        md5(str(data_id).encode("utf-8")).digest())[0] >> PARTITION_SHIFT
    node_ids = [part2node[part]]
    node_counts[node_ids[0]] += 1
    for replica in range(1, REPLICAS):
        while part2node[part] in node_ids:
            part += 1
            if part > PARTITION_MAX:
                part = 0
        node_ids.append(part2node[part])
        node_counts[node_ids[-1]] += 1
desired_count = DATA_ID_COUNT / NODE_COUNT * REPLICAS
print('%d: Desired data ids per node' % desired_count)
max_count = max(node_counts)
over = 100.0 * (max_count - desired_count) / desired_count
print('%d: Most data ids on one node, %.02f%% over' % (max_count, over))
min_count = min(node_counts)
under = 100.0 * (desired_count - min_count) / desired_count
print('%d: Least data ids on one node, %.02f%% under' % (min_count, under))

