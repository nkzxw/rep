# -*- coding: utf-8 -*-  
#pip uninstall apscheduler
#pip install apscheduler==2.1.2

from apscheduler.scheduler import Scheduler
from datetime import datetime
from datetime import timedelta
import urllib
import urllib2
import cookielib
import re
import random

import time
from threading import Timer


cj = cookielib.CookieJar()
opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(cj))
opener.addheaders = [('User-agent','Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)')]
urllib2.install_opener(opener)
scheduler = Scheduler()
scheduler.daemonic = False


def job_function():
    #sleep 一段时间
    print 'Tick! The time is: %s' % datetime.now()
    req = urllib2.Request("http://ccc.bwstor.com.cn/modules/account/ldapcheck.php",urllib.urlencode({"user_name":"zhangxingwei","user_pw":"devialcpboy"}))
    resp = urllib2.urlopen(req)
    #print resp.read()
    req = urllib2.Request("http://ccc.bwstor.com.cn/modules/Attendance/CheckWork.php",urllib.urlencode({"submit1":"下班打卡"}))
    resp = urllib2.urlopen(req)
    str = resp.read()
    #print str
    #Python2中的字符串有两种数据类型：str类型和unicode类型。在3.x系列中，默认均为unicode，无需用 u 进行转码。
    #在python2中字符串默认采用的ASCII编码，如果要显示声明为unicode类型的话，需要在字符串前面加上'u'或者'U'。
    m = re.search(u'打卡成功',str.decode('gb2312'))
    print 'end'
    print m.group(0)
'''
def print_time( enter_time ):
  print "now is", time.time() , "enter_the_box_time is", enter_time
print time.time()
Timer(5,  print_time, ( time.time(), )).start()
Timer(10, print_time, ( time.time(), )).start()
print time.time()
'''
def start_job():
    d1 = datetime.now()
    print d1.ctime()
    rand = random.randint(20, 40)
#https://docs.python.org/2/library/datetime.html
    d2 = d1 + timedelta(minutes = rand, seconds=rand)
    print d2.ctime()
#https://apscheduler.readthedocs.org/en/v2.1.2/cronschedule.html
#scheduler.add_date_job(job_function, '2009-11-06 16:30:05', ['text'])
    scheduler.add_date_job(job_function, d2)
#scheduler.add_interval_job(job_function, minutes=0.1)
#scheduler.add_interval_job(job_function, hours=2, start_date='2013-5-17 18:30')
scheduler.add_cron_job(start_job, day_of_week='mon-fri', hour='8,9,20,21', minute='10,12,13', second='20,21')
scheduler.add_cron_job(start_job, month='9', day='18', hour='8,9,20,21', minute='10,12,13', second='20,21')
print 'scheduler start'
scheduler.start()
