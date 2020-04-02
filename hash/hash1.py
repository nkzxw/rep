# 一致性哈希算法
# 一致性哈希算法是由D. Darger、E. Lehman和T. Leighton 等人于1997年在论文
# Consistent Hashing and Random Trees:Distributed Caching Protocols for Relieving Hot Spots On the World Wide Web
# 首次提出，目的主要是为了解决分布式网络中的热点问题。
# 在其论文中，提出了一致性哈希算法并给出了衡量一个哈希算法的4个指标：
# 平衡性(Balance)
#   平衡性是指Hash的结果能够尽可能分布均匀，充分利用所有缓存空间。
# 单调性(Monotonicity)
#   单调性是指如果已经有一些内容通过哈希分派到了相应的缓冲中，又有新的缓冲加入到系统中。
#   哈希的结果应能够保证原有已分配的内容可以被映射到新的缓冲中去，而不会被映射到旧的缓冲集合中的其他缓冲区。
# 分散性(Spread)
#   分散性定义了分布式环境中，不同终端通过Hash过程将内容映射至缓存上时，因可见缓存不同，Hash结果不一致，相同的内容被映射至不同的缓冲区。
# 负载(Load)
#   负载是对分散性要求的另一个纬度。既然不同的终端可以将相同的内容映射到不同的缓冲区中，
#   那么对于一个特定的缓冲区而言，也可能被不同的用户映射为不同的内容。

# Swift使用该算法的主要目的是在改变集群的node数量时（增加/删除服务器），
# 能够尽可能少地改变已存在key和node的映射关系，以满足单调性。一致性哈希一般两种思路：
# 1.迁移为主要特点(swift初期采用)
# 2.引入虚结点，减少移动为特点(swift现采用)

# 具体步骤如下：
# 1.首先求出每个节点(机器名或者是IP地址)的哈希值，并将其分配到一个圆环区间上（这里取0-2^32）。
# 2.求出需要存储对象的哈希值，也将其分配到这个圆环上。
# 3.从对象映射到的位置开始顺时针查找，将对象保存到找到的第一个节点上。
# 
# 其中这个从哈希到位置映射的圆环，我们就可以理解为何使用术语“Ring”来表示了。哈希环空间上的分布如图1所示：
# 假设在这个环形哈希空间中，Cache5被映射在Cache3和Cache4之间，
# 那么受影响的将仅是沿Cache5逆时针遍历直到下一个Cache（Cache3）之间的对象（它们本来映射到Cache4上）。
# 现在，使用该算法在集群中增加一个node，同时要保证每个节点的数据项数量均衡，代码如下所示，
# 其中node_range_starts表示每个node的数据项的开始位置。

from bisect import bisect_left
from hashlib import md5
from struct import unpack_from

NODE_COUNT = 100
NEW_NODE_COUNT = 101
DATA_ID_COUNT = 10000000

node_range_starts = []
for node_id in range(NODE_COUNT):
    node_range_starts.append(DATA_ID_COUNT /
                             NODE_COUNT * node_id)
new_node_range_starts = []
for new_node_id in range(NEW_NODE_COUNT):
    new_node_range_starts.append(DATA_ID_COUNT /
                                 NEW_NODE_COUNT * new_node_id)
moved_ids = 0
for data_id in range(DATA_ID_COUNT):
    # data_id = str(data_id)
    hsh = unpack_from('>I', md5(str(data_id).encode("utf-8")).digest())[0]
    node_id = bisect_left(node_range_starts,
                          hsh % DATA_ID_COUNT) % NODE_COUNT
    new_node_id = bisect_left(new_node_range_starts,
                              hsh % DATA_ID_COUNT) % NEW_NODE_COUNT
    if node_id != new_node_id:
        moved_ids += 1
percent_moved = 100.0 * moved_ids / DATA_ID_COUNT
print('%d ids moved, %.02f%%' % (moved_ids, percent_moved))


# 4901707 ids moved, 49.02%
