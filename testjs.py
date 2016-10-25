#!/usr/bin/python
#-*- coding: UTF-8 -*-
import os
#pip install PyExecJS
#pip install requests
import execjs
import time
import requests
import json
import re

#执行*.js文件
pathsdir = "testjs.js"
source = open(pathsdir).read().decode('utf-8')
session = requests.session()
http = session.get('http://www.wanplus.com/lol/ranking')
con1 = http.content
#print http.headers
cookies = http.cookies
print cookies
cookie = cookies['wanplus_token']
csrf = execjs.compile(source).call('time33',cookie)
print csrf

headers2={'Accept':'*/*',
'Accept-Encoding':'gzip, deflate, sdch',
'Accept-Language':'zh-CN,zh;q=0.8',
'Connection':'keep-alive',
'Cookie':'',
'Host':'www.wanplus.com',
'Referer':'http://www.wanplus.com/lol/ranking',
'User-Agent':'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.87 Safari/537.36',
'X-Requested-With':'XMLHttpRequest'}

headers2["X-CSRF-Token"] = str(csrf)
newcookies = "gameType=0; wanplus_sid=" + cookies['wanplus_sid'] +"; wanplus_token="+ cookies['wanplus_token'] + "; wanplus_storage="+ cookies['wanplus_storage']
headers2["Cookie"] = newcookies
page = 2
url = "http://www.wanplus.com/ajax/detailranking?country=0&type=1&teamPage=" + str(page) + "&game=2&_gtk=" + str(csrf)
print url 
con2 = session.get(url, headers=headers2).content
print con2
fd = open("testjs.json",'a+')
fd.write(con2)
fd.flush()

url2 = "http://www.wanplus.com/ajax/detailranking?country=0&type=2&playerPage=2&kda=1&game=2&_gtk="
