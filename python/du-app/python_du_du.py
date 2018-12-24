# _*_ coding: utf-8 _*_

"""
python_du.py by xianhu
python2.7 pip install jpype1
python3.5 pip install JPype1_py3-0.5.5.2-cp35-none-win_amd64.whl 
https://www.lfd.uci.edu/~gohlke/pythonlibs/
jre 1.7/1.8
py and java are same amd64 or x86
"""

import re
import rsa
import time
import json
import base64
import logging
import binascii
import requests
import urllib
import os
from jpype import *
import jpype

jarpath = os.path.join(os.path.abspath('.'), 'du2.jar')
#jpype.startJVM('C:/Program Files/Java/jre1.8.0_51/bin/server/jvm.dll', "-ea", "-Djava.class.path=%s" %jarpath)
jvmPath = jpype.getDefaultJVMPath() 
jvmArg = ["-ea","-Djava.class.path=%s" %jarpath]
if not jpype.isJVMStarted(): 
    jpype.startJVM(jvmPath, *jvmArg)

JClass = jpype.JClass('com.shine.support.utils.aq')
instance = JClass()
#jpype.shutdownJVM()

class duLogin(object):
    """
    class of duLogin, to login du.com
    """
    def __init__(self, user_name, pass_word):
        """
        constructor
        """
        self.user_name = user_name
        self.pass_word = pass_word
        self.session = requests.Session()
        self.session.headers.update({   
        "duuuid": "83543a9eba78150",
        "duimei": "862490030196133",
        "duplatform": "android",
        "duchannel":"",
        "duv": "3.5.1", 
        "duloginToken":"", 
        "dudeviceTrait":"1605-A01",
        "User-Agent":"duapp/3.5.1(android;6.0.1)",
        "Host": "m.poizon.com"
        })
        self.li = []

        self.file_out = open("productids.txt", "w+", encoding='utf-8')
        self.file_out2 = open("products.txt", "w+", encoding='utf-8')

        self.file_out.write("start\n")
        self.file_out2.write("start\n")
        return

    def login(self):
        """
        login du.com, return True or False
        """       
        # make post_data
        pass_word = instance.password(self.pass_word)
        sign = instance.Login(self.user_name, pass_word)
        post_data = {
            "userName": self.user_name,
            "password": pass_word,
            "type":"pwd",
            "sourcePage":"",
            "countryCode":"86",
            "sign":sign
        }
        # login
        # proxies = {'http': 'http://localhost:8888', 'https': 'http://localhost:8888'}
        # proxies=proxies
        # cert=('C:/Users/user/Desktop/FiddlerRoot.cer'),
        url = "https://m.poizon.com/users/unionLogin"  
        
        try:
            json_data = self.session.post(url, data=post_data).json()
        except Exception as excep:
            logging.error("detail get_json_data error: %s", excep)
            self.login()
            return
        if json_data["status"] != 200:
            return
        #logging.warning("duLogin: %s", json_data)
        self.loginToken = json_data["data"]["loginInfo"]["loginToken"]
        logging.debug("duLogin: %s", self.loginToken)

    def searchlist(self,  page, limit):
        url = "https://m.poizon.com/search/list?size=[]&title=&typeId=0&catId=0&unionId=0&sortType=0&sortMode=1"
        args = "&page=" + str(page) + "&limit=" + str(limit)
        sign = instance.search(self.loginToken, page, limit)
        url = url + args + "&sign=" + sign
        self.session.headers.update({"duloginToken": self.loginToken})
        
        try:
            json_data = self.session.get(url).json()
        except Exception as excep:
            logging.error("searchlist error: %s", excep)
            return self.searchlist(page, limit)
            
        #logging.debug("duLogin get_json_data: %s", str(json_data))
        #json_data = json.loads(response.text)
        #logging.debug("duLogin json_data: %s", json_data)
        if json_data["status"] != 200:
            return 1
        list1 = json_data["data"]["productList"]
        li = []
        for i in list1:
            li.append(i["productId"])
            self.detail(i["productId"])
        if li:
            logging.debug("duLogin get_json_data: %s", li)
            for i in li:
                self.file_out.write(str(i))
                self.file_out.write(' ')
            self.file_out.flush()
            return 1
        else :
            logging.debug("duLogin get_json_data empty")
            return 0

            
    def detail(self, productId):
        url = "https://m.poizon.com/product/detail?productId=" + str(productId) + "&isChest=1"
        sign = instance.detail(self.loginToken, productId)
        url = url + "&sign=" + sign
        self.session.headers.update({"duloginToken": self.loginToken})
        
        try:
            response = self.session.get(url) 
        except Exception as excep:
            logging.error("detail get_json_data error: %s", excep)
            self.detail(productId)
            return

        #logging.debug("get_json_data error: " + response)
        json_data = response.json()
        #setting = json.loads(response.text)
        if json_data["status"] != 200:
            return
        #logging.debug("duLogin get_detail_data: %s", self.parse_product_data(response.text))
        try:
            dic = self.parse_product_data(json_data['data'])
        except Exception as excep:
            logging.debug("get_json_data error: " + str(json_data))
            return
        self.file_out2.write(str(productId)+","+str(dic)+"\n")
        self.file_out2.flush()

    def parse_product_data(self, setting):
        try:
            price = setting['item']["price"]/100
        except Exception as excep:
            price = 0
        exchangeDesc = setting["exchangeDesc"]   
        detail = setting['detail']
        title = detail["title"]   
        color = detail["color"]   #反斜线分隔开
        sellDate = detail["sellDate"]
        authPrice = detail["authPrice"]/100
        articleNumber = detail["articleNumber"]
        sizeList = detail["sizeList"]
        soldNum  = detail["soldNum"]
        logoUrl = detail["logoUrl"]   
        imageAndText = setting["imageAndText"]   
        #logging.debug("duLogin imageAndText: %s", imageAndText)
        b = "<p>(?P<result>.*?)<\/p>"
        B = re.compile(b, re.IGNORECASE)
        #\n去除换行，可用其他代替，或不去除
        try:
            text = B.search(imageAndText).group("result").replace('<br/>','')
        except Exception as excep:
            text = "nodetil"
        #text = re.search(r"\<\p\>.+\<\/p\>", imageAndText)
        #logging.debug ([title, price/100 , sizeList, color, articleNumber, authPrice/100 ,sellDate, exchangeDesc, logoUrl, text, soldNum])
        return [ title, price , sizeList, color, articleNumber, authPrice ,sellDate, exchangeDesc, logoUrl,  text, soldNum]


if __name__ == "__main__":

    logging.basicConfig(level=logging.DEBUG, format="%(asctime)s\t%(levelname)s\t%(message)s", filename='./loggmsg.log',filemode='w')
    du = duLogin("13161267107", "qqbwstor")
    du.login()
    du.searchlist(501, 20)
    i = 1
    res = 1
    while(res !=1 ):
        res = du.searchlist(i, 20)
        i += 1
    #jpype.shutdownJVM()

