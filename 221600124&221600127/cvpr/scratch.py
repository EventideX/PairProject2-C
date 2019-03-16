import requests
import re
url='http://openaccess.thecvf.com/CVPR2018.py'
response=requests.get(url)
html=response.text
find=re.findall(r'<dt class="ptitle"><br><a href="(.*?)">(.*?)</a></dt>',html,re.S)
fb=open('result.txt','w',encoding='utf-8')
i=0
for each in find:
    each_url,each_title=each
    each_url='http://openaccess.thecvf.com/'+each_url
    each_response = requests.get(each_url)
    each_response.encoding="utf-8"
    each_html = each_response.text
    each_abstract = re.findall(r'<div id="abstract" >\n(.*?)</div>', each_html,re.S)[0]
    fb.write('%s\n' % i)
    fb.write('Title: %s\n'% each_title)
    fb.write('Abstract: %s\n\n\n' % each_abstract)
    i=i+1