# _*_ coding: utf-8 _*_
import os
import logging
import logging.config

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DEBUG = False #标记是否在开发环境

#给过滤器使用的判断
class RequireDebugTrue(logging.Filter):
    #实现filter方法
    def filter(self, record):
        return DEBUG


LOGGING = {
    #基本设置
    'version':1, #日志级别
    'disable_existing_loggers':False, #是否禁用现有的记录器
    
    #日志格式集合
    'formatters':{ 
        #标准输出格式
        'standard':{
            #[具体时间][线程名:线程ID][级别名称] 日志内容
            'format':'[%(asctime)s][%(threadName)s:%(thread)d][%(levelname)s]\n%(message)s'
        }
    },
    
    #过滤器
    'filters':{
        'require_debug_true': {
            '()': RequireDebugTrue,
        }
    },
    
    #处理器集合
    'handlers':{ 
        #输出到控制台
        'console':{
            'level':'DEBUG',    #输出信息的最低级别
            'class':'logging.StreamHandler',
            'formatter':'standard', #使用standard格式
            'filters': ['require_debug_true',], #仅当 DEBUG = True 该处理器才生效
        },
        #输出到文件
        'log':{
            'level':'DEBUG',
            'class':'logging.handlers.RotatingFileHandler',
            'formatter':'standard',
            'filename':os.path.join(BASE_DIR, 'debug.log'), #输出位置
            'maxBytes':1024*1024*5, #文件大小 5M
            'backupCount': 5, #备份份数
            'encoding': 'utf8', #文件编码
        },
    },
    
    #日志管理器集合
    'loggers':{
        #管理器
        'default':{
            'handlers':['console', 'log'],
            'level':'DEBUG',
            'propagate':True, #是否传递给父记录器
        },
    },
    
    "root":{
        'handlers':['console', 'log'],
        'level':'DEBUG',
    }
}


#加载前面的标准配置
logging.config.dictConfig(LOGGING)

#日志记录装饰器
def recode_log(func):
    #获取loggers其中的一个日志管理器
    logger = logging.getLogger("default")
    def warpper(*args, **kw):
        #记录开始运行时间
        logger.debug("start %s" ,func.__name__)
        #运行方法
        func()
        #记录结束运行时间
        logger.debug("end %s" ,func.__name__)
    return warpper
    
#测试方法
@recode_log
def test():
    print("run test")
 
#获取loggers其中的一个日志管理器
logger = logging.getLogger("default")
logger.debug("debug message")
logger.info("info message")
logger.warn("warn message")
logger.error("error message")
logger.critical("critical message")
logging.debug("55555555555")
