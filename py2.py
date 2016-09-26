#-*_coding:utf-8-*-
import time
import requests
import json
import re
from xtls.util import BeautifulSoup
import logging

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
	#return con2

def savecontent(filename, con):
	fp = open(filename,'a+')
	fp.write(con)
	fp.close()

def save_one_page_movie(link):
	con2 = get_and_sleep(link)
	#savecontent("test"+str(page)+"a.html",con2)
	soup2 = BeautifulSoup(con2)
	list2 = soup2.find_all("a", attrs={"class": "movie-box"})
	for la2 in list2:#每个作品
		link2 = la2.find("span")
		list3 = link2.find_all("date")
		savecontent("test" + str(page) + "a.html","\n" + link2.contents[0])#作品名
		savecontent("test" + str(page) + "a.html","\n" + list3[0].get_text())#番号
		savecontent("test" + str(page) + "a.html","\n" + list3[1].get_text())#日期
		link3 = la2["href"]
		con3 = get_and_sleep(link3)
		soup3 = BeautifulSoup(con3)
		movie = soup3.find("div", attrs={"class": "col-md-3 info"}).find_all("p")[2]
		#m1 = movie.contents[0]
		#print type(m1)
		#print m1.get_text()
		duration = movie.get_text()	
		#print duration
		savecontent("test" + str(page) + "a.html","\n" + duration)#时长
		savecontent("test" + str(page) + "a.html","\n" + link3)#链接
		#break #break on one page one movie
	#return next page link
	next = soup2.find("a", attrs={"name": "nextpage"})
	if(next is not None):
		nextlink = next["href"]
		return nextlink

if __name__ == '__main__':
	session = requests.session()
	logging.basicConfig(
	filename='server.log',
	level=logging.INFO,
	format='%(asctime)s,%(levelname)s,%(filename)s:%(lineno)d,%(threadName)s:%(message)s', 
	datefmt='[/%Y/%m%d-%H:%M:%S]')
	try:
		for page in range(1,200): #不含最大值
			#print page
			con1 = get_and_sleep('https://avmo.pw/cn/actresses/page/' + str(page))
			#savecontent("test"+str(page)+".html",con1)
			#从中找出名字和链接
			soup1 = BeautifulSoup(con1)
			list=soup1.find_all("a", attrs={"class": "avatar-box text-center"})
			for la in list:#每个演员
				savecontent("test" + str(page) + "a.html","\n@@@" + la.find("span").get_text())	#演员名@@@开始
				link = la["href"]#la.get("href")
				#一页作品
				nextlink = save_one_page_movie(link)
				#下一页
				while (nextlink is not None):
					nextlink = save_one_page_movie("https://avmo.pw" + nextlink)
				#break #break one actor
	except:
		logging.exception("exception")
