# 现在假设增加一个节点提供负载能力，不过得重新分配数据项到新的节点上。
# NODE_COUNT--->NEW_NODE_COUNT



from hashlib import md5
from struct import unpack_from  

NODE_COUNT = 100
NEW_NODE_COUNT = 101
DATA_ID_COUNT = 10000000

moved_ids = 0
for data_id in range(DATA_ID_COUNT):
    data_id = str(data_id)
    hsh = unpack_from('>I', md5(data_id.encode("utf-8")).digest())[0]
    node_id = hsh % NODE_COUNT
    new_node_id = hsh % NEW_NODE_COUNT
    if node_id != new_node_id:
        moved_ids += 1
percent_moved = 100.0 * moved_ids / DATA_ID_COUNT
print('%d ids moved, %.02f%%' % (moved_ids, percent_moved))


#9900989 ids moved, 99.01%