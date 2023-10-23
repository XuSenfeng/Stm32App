#include "jiao_app.h"
//存储着显示的内容
char * jiao_strs[MAX_STR_SHOW] = {"每个人都不是一座孤岛，一个人必须是这世界上最坚固的岛屿，然后才能成为大陆的一部分。——海明威", 
"总之岁月漫长，然而值得等待。——村上春树",
"凡是过去，皆为序章。——莎士比亚",
"少年与爱永不老去，即便披荆斩棘，丢失怒马鲜衣。 ——莫峻", 
"没有一个冬天不可逾越，没有一个春天不会来临。——南方周末", 
"浅水是喧哗的，深水是沉默的。——雪莱", 
"四方食事，不过一碗人间烟火。——小灯泡儿", 
"天可补，海可填，南山可移。日月既往，不可复追。——曾国藩", 
"我走得很慢，但我从不后退。——林肯", 
"花看半开，酒饮微醺。——《菜根谭》", 
"花开如火，也如寂寞。——顾城", 
"愿你一生努力，一生被爱。想要的都拥有，得不到的都释怀。——八月长安", 
"纵使黑夜吞噬了一切，太阳还可以重新回来。——汪国真", 
"一星陨落，黯淡不了星空灿烂；一花凋零，荒芜不了整个春天。——巴尔扎克", 
"我有一瓢酒，可以慰风尘。——韦应物", 
"鲸落海底，哺暗界众生十五年。——加里·斯奈德", 
"我荒废了时间，时间便把我荒废了。——莎士比亚", 
"只有流过血的手指，才能弹出世间的绝唱。——泰戈尔", 
"夜暗方显万颗星，灯明始见一缕尘。——七堇年", 
"月缺不改光，剑折不改刚。——梅尧臣", 
"活着不一定要鲜艳，但一定要有自己的颜色。——张曙光", 
"掉头一去是风吹黑发，回首再来已雪满白头。——余光中", 
"万物皆有裂痕，那是光照进来的地方。——莱昂纳德·科恩", 
"每个人都在愤世疾俗，每个人又都在同流合污。——《自由在高处》", 
"你生而有翼，为何竟愿一生匍匐前进，形如虫蚁。——贾拉尔·阿德丁·鲁米", 
"浮名浮利，虚苦劳神。叹隙中驹，石中火，梦中身。——苏轼", 
"少年安得长少年，海波尚变为桑田。——李贺", 
"谁终将声震人间，必长久深自缄默；谁终将点燃闪电，必长久如云漂泊。——尼采", 
"容颜会老去，四季不会停。那些散碎在笔尖的光阴，寂静欢喜。——陆苏",
"当华美的叶片落尽，生命的脉络才历历可见。——聂鲁达", 
"日出之美便在于它脱胎于最深的黑暗。——辛夷坞", 
"一盏一直亮着的灯，你不会去注意，但是如果它一亮一灭，你就会注意到。——刘墉", 
"这世界是如此喧哗，让沉默的人显得有点傻。——李宗盛", 
"如果结果不如你所愿，就在尘埃落定前奋力一搏。——《夏目友人帐》", 
"宁鸣而死，不默而生。——范仲淹",
"人生没有白走的路，每一步都算数。——李宗盛", 
"所谓浮躁，也就是时时刻刻，希望以最短的时间，博取最多的存在感、优越感和自我认同。——张佳玮", 
"从前的日色变得慢，车，马，邮件都慢，一生只够爱一个人。——木心", 
"有些事不是看到了希望才去坚持，而是因为坚持才会看到希望。——《十宗罪》", 
"只有用水将心上的雾气淘洗干净，荣光才会照亮最初的梦想。——《百年孤独》", 
"人生的旅程就是这样，用大把时间迷茫，在几个瞬间成长。——瑞卡斯", 
"成功就是从失败到失败，也依然不改热情。——丘吉尔", 
"方向是比速度更重要的追求。——白岩松", 
"在每个死胡同的尽头，都有另一个维度的天空，在无路可走时迫使你腾空而起，那就是奇迹。——廖一梅", 
"给自己一点时间，别害怕重新开始。——威尔·鲍温", 
"没有不可治愈的伤痛，没有不能结束的沉沦，所有失去的，会以另一种方式归来。——约翰·肖尔斯", 
"人活一生，值得爱的东西很多，不要因为一个不满意，就灰心。——路遥", 
"虽然明天还会有新的太阳，但永远不会有今天的太阳了。——林清玄", 
"凡心所向，素履所往，生如逆旅，一苇以航。——《尘曲》", 
"醉过才知酒浓，爱过才知情重。你不能做我的诗，正如我不能做你的梦。——胡适", 

};
