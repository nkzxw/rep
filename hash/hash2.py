# 引入虚拟节点(Partition)
# 
# 考虑到哈希算法在node较少的情况下，改变node数会带来巨大的数据迁移。
# 为了解决这种情况，一致性哈希引入了“虚拟节点”的概念： 
# “虚拟节点”是实际节点在环形空间的复制品，一个实际节点对应了若干个“虚拟节点”，“虚拟节点”在哈希空间中以哈希值排列。
# 引入了“虚拟节点”后，映射关系就从【object--->node】转换成了【object--->virtual node---> node】。
# 对100个node细分为1000个vnode，使用vnode_range_starts来指定vnode的开始范围，vnode2node表示vnode到node的指派，
# 然后增加一个node，完成vnode的重新分配，并计算所移动的数据项：

from bisect import bisect_left
from hashlib import md5
from struct import unpack_from
from time import time

NODE_COUNT = 100
DATA_ID_COUNT = 10000000
VNODE_COUNT = 1000
begin = time()

vnode_range_starts = []
vnode2node = []
for vnode_id in range(VNODE_COUNT):
    vnode_range_starts.append(DATA_ID_COUNT /
                              VNODE_COUNT * vnode_id)
    vnode2node.append(vnode_id % NODE_COUNT)
new_vnode2node = list(vnode2node)
new_node_id = NODE_COUNT
NEW_NODE_COUNT = NODE_COUNT + 1
vnodes_to_reassign = VNODE_COUNT / NEW_NODE_COUNT
while vnodes_to_reassign > 0:
    for node_to_take_from in range(NODE_COUNT):
        for vnode_id, node_id in enumerate(new_vnode2node):
            if node_id == node_to_take_from:
                new_vnode2node[vnode_id] = new_node_id
                vnodes_to_reassign -= 1
                if vnodes_to_reassign <= 0:
                    break
        if vnodes_to_reassign <= 0:
            break


moved_ids = 0
for data_id in range(DATA_ID_COUNT):
    # data_id = str(data_id)
    hsh = unpack_from('>I', md5(str(data_id).encode("utf-8")).digest())[0]
    vnode_id = bisect_left(vnode_range_starts,
                         hsh % DATA_ID_COUNT) % VNODE_COUNT
    node_id = vnode2node[vnode_id]
    new_node_id = new_vnode2node[vnode_id]
    if node_id != new_node_id:
        moved_ids += 1
percent_moved = 100.0 * moved_ids / DATA_ID_COUNT
print('%d ids moved, %.02f%%' % (moved_ids, percent_moved))
print('%d seconds pass ...' % (time() - begin))


# 90108 ids moved, 0.90%

