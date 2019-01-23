# _*_ coding: utf-8 _*_

import re
import time
import logging
import requests
import os
from bs4 import BeautifulSoup


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
        "origin": "http://i.mypep.cn",
        "upgrade-insecure-requests": "1",
        "user-agent": "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/71.0.3578.98 Safari/537.36"
        })

    def login(self):
        """
        login du.com, return True or False
        """
        # make post_data
        post_data = {
        "_token": "85e1inSOZhMeGibY4G4X1P5Ny4AsA6tRFhJZ5Stp",
        "regName": self.user_name,
        "passwd": self.pass_word,
        "pepact": "login",
        "authKey": "1eceed838aaeb5047607730f5e151e08",
        "unique": 0,
        "callBackUrl": "http://i.mypep.cn/pep/callback?next=http%3A%2F%2Fwww.mypep.cn%2F&remember=0",
        "remember_me": 0
        }
        url = "https://user.mypep.com.cn/ssoapi.php"  
        try:
            json_data = self.session.post(url, data=post_data)
        except Exception as excep:
            logging.error("get data error: %s", excep)
            return self.login()
        logging.debug("duLogin: %s", json_data.status_code)
        return self

    def searchlist(self,  page, limit):
        return 0
            
    def detail_list(self, productId):
        url = "http://v.mypep.cn/info?attr=1&sid=" + str(productId) 
        try:
            response = self.session.get(url) 
        except Exception as excep:
            logging.error("get data error: %s", excep)
            return

        #logging.debug("get_json_data: " + response.text)
        if(response.text):
            soup = BeautifulSoup(response.text, 'html.parser')
            dd = soup.find_all("dd")
            for film_item in dd:
                #logging.debug("get_json_data: %s", film_item)
                url = film_item.find_all('a')[0]
                film_detail_url = url.get('href')
                film_detail_title = url.contents[0]
                logging.debug("response.text: %s, %s",film_detail_url, film_detail_title)
                self.detail(film_detail_url, film_detail_title)
        
    def detail(self, base, name):
        #base = "http://v.mypep.cn/play/" + Id
        try:
            response = self.session.get(base) 
        except Exception as excep:
            logging.error("get data error: %s", excep)
            return
        url = self.parse_product_data(response.text)
        url = "http://v.mypep.cn" + url     
        logging.debug("get vidoe data: " + url)
        self.session.headers.update({"Referer": base})
        try:
            r = self.session.get(url,stream=True) 
            f = open(name + ".mp4", "wb")
            for chunk in r.iter_content(chunk_size=512):
                if chunk:
                    f.write(chunk)
            f.close()
        except Exception as excep:
            logging.error("get data error: %s", excep)
        return
       
    def parse_product_data(self, setting):
        b = "video_url: \"(?P<result>.*?)\""
        B = re.compile(b)
        try:
            text = B.search(setting).group("result")
        except Exception as excep:
            text = "no-url"
            logging.error("parse_product_data: %s" , excep)
        return text


if __name__ == "__main__":

    logging.basicConfig(level=logging.DEBUG, format="%(asctime)s\t%(levelname)s\t%(message)s", filename='./loggmsg.log',filemode='w')
    #filemode = w覆盖写，filemode = a追加
    du = duLogin("zhou846", "zhou@1986727").login()
    du.detail_list(23246)
