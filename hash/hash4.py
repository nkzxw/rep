# 预设合理的虚结点数
# 
# 现在已构建好了一致性哈希ring的原型。
# 但是存在一个问题，以上例子中，1000个虚结点对应着100个结点，结点变动时，虚结点就需要重新分配到结点。
# 当100个结点扩展到1001个结点时，此时至少有一个结点分配不到虚结点，那么就需要再增加虚结点数，
# 而虚结点是与数据项对应的哈希关系，如果改变了虚节点数，那么就需要重新分配所有的数据项，这将导致移动大量的数据。
# 所以在设置虚结点数的时候，需要对系统预期的规模做充分考虑，
# 假如集群的规模不会超过6000个结点，那么可以将虚结点数设置为结点数的100倍。
# 这样，变动任意一个结点的负载仅影响1%的数据项。此时有6百万个vnode数，使用2bytes来存储结点数(0~65535)。
# 基本的内存占用是6*106*2bytes=12Mb，对于服务器来说完全可以承受。
# 
# 
# 在此，引入了2个概念：
# 在swift中，为了区分vnode和node，将vnode称为partition。
# 位操作代替取模操作
# 此外，在计算机中使用位操作来确定partition的位置比取模更快。所以，在此引入了partition power的概念。
#  继续改进ring的代码，设有65536个node(2^16)，有128（2^7）倍个partition数(2^23)。
# 由于MD5码是32位的，使用PARTITION_SHIFT(等于32- PARTITION_POWER)将数据项的MD5哈希值映射到partition的2^23的空间中。


from array import array
from hashlib import md5
from struct import unpack_from

PARTITION_POWER = 23
PARTITION_SHIFT = 32 - PARTITION_POWER
NODE_COUNT = 65536
DATA_ID_COUNT = 100000000

part2node = array('H')
for part in range(2 ** PARTITION_POWER):
    part2node.append(part % NODE_COUNT)
node_counts = [0] * NODE_COUNT
for data_id in range(DATA_ID_COUNT):
    # data_id = str(data_id)
    part = unpack_from('>I',
        md5(str(data_id).encode("utf-8")).digest())[0] >> PARTITION_SHIFT
    node_id = part2node[part]
    node_counts[node_id] += 1
desired_count = DATA_ID_COUNT / NODE_COUNT
print('%d: Desired data ids per node' % desired_count)
max_count = max(node_counts)
over = 100.0 * (max_count - desired_count) / desired_count
print('%d: Most data ids on one node, %.02f%% over' % \
    (max_count, over))
min_count = min(node_counts)
under = 100.0 * (desired_count - min_count) / desired_count
print('%d: Least data ids on one node, %.02f%% under' % \
    (min_count, under))