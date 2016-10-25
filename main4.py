#-*_coding:utf-8-*-
import time
import requests #pip install requests[security]
import json
import re
from xtls.util import BeautifulSoup
import logging
import Queue
import threading
# pip install pyopenssl ndg-httpsclient pyasn1 --upgrade
# import datetime

# hosts = ["https://www.sogou.com/", "http://www.hupu.com/"]
in_queue = Queue.Queue()
con_queue = Queue.Queue()

def get_and_sleep(link):
    #con2 = session.get(link).content
    #time.sleep(0.5)
    msg = None   
    for i in [1,2,3]:
        try:
            con2 = requests.get(link, timeout=30)
            #print type(con2)
            return con2.content
        except Exception as e:
            logging.exception('Got error when requesting URL "' + link + '": ' + str(e) + '\n')
            if i == 3 :#Failed requesting from URL
                #logging.exception('{0.filename}@{0.lineno}: Failed requesting from URL "{1}" ==> {2}\n'.format(inspect.getframeinfo(inspect.currentframe()), link, e))
                logging.exception('Failed requesting from URL "' + link + '": ' + str(e) + '\n')
                raise e
            time.sleep(10*(i-1))

class thread_url(threading.Thread):
    """Threaded Url Grab"""

    def __init__(self, queue1, queue2):
        threading.Thread.__init__(self)
        self.url_queue = queue1
        self.con_queue = queue2

    def run(self):
        while True:
            # grabs host from url_queue
            # logging.error("getting")
            host = self.url_queue.get()
            # url = host["value"]
            # logging.error("got " + host["value"])
            try:
                chunk = get_and_sleep(host["value"])
            except:
                logging.exception("exception")
            #item = {"type":host["type"],"value":chunk}
            host["value"] = chunk
            #if(item[type])
            self.con_queue.put(host)
            # signals to url_queue job is done
            # self.url_queue.task_done()


class thread_datamine(threading.Thread):
    """Threaded Url Grab"""

    def __init__(self, queue1, queue2):
        threading.Thread.__init__(self)
        self.url_queue = queue1
        self.con_queue = queue2
        self.operator = {"1":self.jia,"2":self.jian,"3":self.cheng,"4":self.chu}
        self.fp = open(self.getName()+".html",'a+')
        #self.fp.truncate(0)

    #def savecontent(self, con):
        ## logging.error("calling savecontent" + con)
    #    self.fp.write(con)
    #def flush():
    #    self.fp.flush()
        
    def jia(self, con1):
        # https://avmo.pw/cn/actresses//page/2
        # logging.error("call jia")
        soup1 = BeautifulSoup(con1["value"])
        list = soup1.find_all("a", attrs={"class": "avatar-box text-center"})
        for la in list:#每个演员
            #savecontent("\n@@@" + la.find("span").get_text())    #演员名@@@开始
            actor = la.find("span").get_text()
            link = la["href"]#la.get("href")
            item = {"type":"2","value":link,"actor":actor}
            #con1["value"] = link
            #con1["type"] = "2"
            #con1["actor"] = actor
            # logging.error("jia put" + link)
            self.url_queue.put(item)
        # 下一页
        next = soup1.find("a", attrs={"name": "nextpage"})
        if(next is not None):
            nextlink = next["href"]
            # logging.error("jia next =:" + nextlink)
            item = {"type":"1","value":"https://avmo.pw" + nextlink}
            self.url_queue.put(item)

    def jian(self,con2):
        # logging.error("call jian")
        soup2 = BeautifulSoup(con2["value"])
        list2 = soup2.find_all("a", attrs={"class": "movie-box"})
        for la2 in list2:#每个作品
            link2 = la2.find("span")
            # list3 = link2.find_all("date")
            #savecontent("\n" + link2.contents[0])#作品名
            #savecontent("\n" + list3[0].get_text())#番号
            #savecontent("\n" + list3[1].get_text())#日期
            link3 = la2["href"]
            # logging.error("jian get " + link3)
            con3 = get_and_sleep(link3)
            # logging.error("jian got " + link3)
            soup3 = BeautifulSoup(con3)
            title = soup3.find_all("div", attrs={"class": "container"})[1].find("h3")
            movie = soup3.find("div", attrs={"class": "col-md-3 info"}).find_all("p")
            #m1 = movie.contents[0]
            #print type(m1)
            #print m1.get_text()
            # duration = movie.get_text()    
            # print duration[2]
            # logging.error("jian get " + title.get_text())
            # logging.error("jian get " + movie[0].get_text())
            # logging.error("jian get " + movie[1].get_text())
            # logging.error("jian get " + movie[2].get_text())
            # logging.error("call savecontent")
            self.fp.write("\n" + link2.contents[0] + "\n" + movie[0].get_text() + "\n" + movie[1].get_text() + "\n" + movie[2].get_text() + "\n演员：" + con2["actor"])
            #savecontent("\n" + link3)#链接
        #flush()
        self.fp.flush()
        next = soup2.find("a", attrs={"name": "nextpage"})
        if(next is not None):
            nextlink = next["href"]
            # logging.error("jian next=" + nextlink)
            con2["value"] = "https://avmo.pw" + nextlink
            #item = {"type":"2","value":"https://avmo.pw" + nextlink, "actor":con2["actor"]}
            self.url_queue.put(con2)
            
    def cheng(x):  
        print x
    
    def chu(x):  
        print x 
        
    def run(self):
        while True:
            # grabs host from url_queue
            # logging.error("getting")
            item = self.con_queue.get()
            # logging.error("got "+ item["type"])
            #print type(item["value"])
            try:
                self.operator.get(item["type"])(item)  
            except:
                logging.exception("exception")

            # signals to url_queue job is done
            # self.con_queue.task_done()
 
def start_set():
    logging.basicConfig(
    filename='server.log',
    level=logging.ERROR,
    format='%(asctime)s,%(levelname)s,%(filename)s:%(lineno)d,%(threadName)s:%(message)s', 
    datefmt='[/%Y/%m%d-%H:%M:%S]')
    

def start_thread():
    for i in range(4):
        t = thread_url(in_queue, con_queue)
        #t.setDaemon(True)
        t.start()
    for i in range(8):
        dt = thread_datamine(in_queue, con_queue)
        #dt.setDaemon(True)
        dt.start()
        # populate url_queue with data


def main():
    # spawn a pool of threads, and pass them url_queue instance
    start_set()
    host = "https://avmo.pw/cn/actresses/page/1"
    item = {"type":"1","value":host}
    #print "type=:" + str(item["type"])
    #print "value=:" + str(item["value"])
    #print type(item["value"])
    in_queue.put(item)
    start_thread()
    # wait on the url_queue until everything has been processed
    # in_queue.join()
    # con_queue.join()
#start = time.time()
main()
#print("Elapsed Time: %s" % (time.time() - start))
