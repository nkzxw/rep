# https://www.cnblogs.com/yuxc/archive/2012/06/22/2558312.html
# https://zhuanlan.zhihu.com/p/60963885
# https://github.com/openstack/swift/blob/master/swift/common/ring/ring.py

# 普通Hash算法与场景分析
#
# 先来看一个简单的例子假设我们手里有N台存储服务器（以下简称node），
# 打算用于图片文件存储，为了使服务器的负载均衡，
# 需要把对象均匀地映射到每台服务器上，通常会使用哈希算法来实现，计算步骤如下：
# 1.计算object的hash值Key
# 2.计算Key mod N值
# 有N个存储节点，将Key模N得到的余数就是该Key对应的值需要存放的节点。
# 比如，N是2，那么值为0、1、2、3、4的Key需要分别存放在0、1、0、1和0号节点上。
# 如果哈希算法是均匀的，数据就会被平均分配到两个节点中。
# 如果每个数据的访问量比较平均，负载也会被平均分配到两个节点上。
# 但是，当数据量和访问量进一步增加，两个节点无法满足需求的时候，
# 需要增加一个节点来服务客户端的请求。
# 这时，N变成了3，映射关系变成了Key mod (N+1)，
# 因此，上述哈希值为2、3、4的数据需要重新分配（2->server 2，3 -> server 0，4 -> server 1）。
# 如果数据量很大的话，那么数据量的迁移工作将会非常大。
# 当N已经很大，从N加入一个节点变成N+1个节点的过程，会导致整个哈希环的重新分配，
# 这个过程几乎是无法容忍的，几乎全部的数据都要重新移动一遍。
# 我们举例说明，假设有100个node的集群，将10^7项数据使用md5 hash算法分配到每个node中，Python代码如下：

from hashlib import md5
from struct import unpack_from

NODE_COUNT = 100
DATA_ID_COUNT = 10000000
# DATA_ID_COUNT = 10

node_counts = [0] * NODE_COUNT
for data_id in range(DATA_ID_COUNT):
    # This just pulls part of the hash out as an integer
    md = md5(str(data_id).encode("utf-8")).digest()
    # mdstr = md5(str(data_id).encode("utf-8")).hexdigest()
    # print('hash = ', mdstr, md)
    hsh = unpack_from('>I', md)[0]
    node_id = hsh % NODE_COUNT
    node_counts[node_id] += 1
desired_count = DATA_ID_COUNT / NODE_COUNT
print('%d: Desired data ids per node' % desired_count)
max_count = max(node_counts)
over = 100.0 * (max_count - desired_count) / desired_count
print('%d: Most data ids on one node, %.02f%% over' % (max_count, over))
min_count = min(node_counts)
under = 100.0 * (desired_count - min_count) / desired_count
print('%d: Least data ids on one node, %.02f%% under' % (min_count, under))
