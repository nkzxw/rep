# 固化虚节点到数据项的映射
# 
# 由于虚节点个数在集群的整个生命周期中是不会变化的，
# 它与数据项的映射关系不会发生变化，
# 改变的仅是vnode与node的映射关系，所以需对以上代码进行优化。

from struct import unpack_from
from hashlib import md5
from time import time

NODE_COUNT = 100
DATA_ID_COUNT = 10000000
VNODE_COUNT = 1000

begin = time()
vnode2node = []
for vnode_id in range(VNODE_COUNT):
    vnode2node.append(vnode_id % NODE_COUNT)
new_vnode2node = list(vnode2node)
new_node_id = NODE_COUNT
vnodes_to_assign = VNODE_COUNT / (NODE_COUNT + 1)
while vnodes_to_assign > 0:
    for node_to_take_from in range(NODE_COUNT):
        for vnode_id, node_id in enumerate(vnode2node):
            if node_id == node_to_take_from:
                vnode2node[vnode_id] = new_node_id
                vnodes_to_assign -= 1
                if vnodes_to_assign <= 0:
                    break
        if vnodes_to_assign <= 0:
            break
moved_id = 0
for data_id in range(DATA_ID_COUNT):
    # data_id = str(data_id)
    hsh = unpack_from('>I', md5(str(data_id).encode("utf-8")).digest())[0]
    vnode_id = hsh % VNODE_COUNT#(1)
    node_id = vnode2node[vnode_id]
    new_node_id = new_vnode2node[vnode_id]
    if node_id != new_node_id:
        moved_id += 1
percent_moved = 100.0 * moved_id / DATA_ID_COUNT
print('%d ids moved, %.02f%%' % (moved_id, percent_moved))
print('%d seconds pass ...' % (time() - begin))
