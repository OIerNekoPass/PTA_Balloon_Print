import json
import requests
import codecs

# 获取部分

problem_set_id = "" # problem set id为比赛id
url = "https://pintia.cn/api/xcpc/distributions?problem_set_id="+problem_set_id+"&filter=%7B%22state%22%3A%22NOT_DELIVERED%22%7D"
# filter后只接收未分发的气球

cookie = {
	# 此处填写cookie，格式如下
	# "PTASession": "xxxxxx"
}
header = {
	"Accept": "application/json;charset=UTF-8",
	"Content-Type": "application/json;charset=UTF-8",
}
resp = requests.get(url, cookies=cookie, headers=header)
print(resp.status_code)
msg = json.loads(resp.content)

file = codecs.open('print_info/printer.txt','w','GBK')
file.write('BALLOON_STATUS')
for balloon in msg["distributions"]:
	#气球id
	str_str = "\n"+balloon["id"]
	file.write(str_str.encode('UTF-8',errors='ignore').decode('UTF-8',errors='ignore'))
	
	#teamname
	str_str = "\n"+msg["teamById"][balloon["teamId"]]["name"]
	file.write(str_str.encode('UTF-8',errors='ignore').decode('UTF-8',errors='ignore'))
	#teampos
	str_str = "\n"+msg["teamById"][balloon["teamId"]]["remark"]
	file.write(str_str.encode('UTF-8',errors='ignore').decode('UTF-8',errors='ignore'))
	#promblemname
	str_str = "\n"+msg["problemSetProblemById"][balloon["problemSetProblemId"]]["label"]
	file.write(str_str.encode('UTF-8',errors='ignore').decode('UTF-8',errors='ignore'))
	#ballooncolor
	str_str = "\n"+msg["balloonByProblemSetProblemId"][balloon["problemSetProblemId"]]["color"]
	file.write(str_str.encode('UTF-8',errors='ignore').decode('UTF-8',errors='ignore'))
	#firstsolve
	str_str = "\n"+str(balloon["problemSetProblemFirstObtained"])
	file.write(str_str.encode('UTF-8',errors='ignore').decode('UTF-8',errors='ignore'))
	#spilt
	file.write("\n----------------")

#我的电脑python不知道为啥默认字符串编码是gbk
#所以这里用了一堆decode保证别的电脑最后也能输出gbk格式的字符串到文件里
#QAQ

file.close()


#确认派送完成
deliver_url = "https://pintia.cn/api/xcpc/distributions?problem_set_id="+problem_set_id
for balloon in msg["distributions"]:
	distributionIds = balloon["id"]
	# print(distributionIds)
	data = {
		"distributionIds":[distributionIds],
		"delivered":True
	}
	resp = requests.put(deliver_url, data=json.dumps(data), headers=header,cookies=cookie) # 注意是put
	# print(resp.status_code)

print("---获取完成---")