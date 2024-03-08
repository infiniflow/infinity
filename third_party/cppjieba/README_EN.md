# CppJieba [简体中文](README.md)

[![Build Status](https://travis-ci.org/yanyiwu/cppjieba.png?branch=master)](https://travis-ci.org/yanyiwu/cppjieba) 
[![Author](https://img.shields.io/badge/author-@yanyiwu-blue.svg?style=flat)](http://yanyiwu.com/) 
[![Platform](https://img.shields.io/badge/platform-Linux,%20OS%20X,%20Windows-green.svg?style=flat)](https://github.com/yanyiwu/cppjieba)
[![Performance](https://img.shields.io/badge/performance-excellent-brightgreen.svg?style=flat)](http://yanyiwu.com/work/2015/06/14/jieba-series-performance-test.html) 
[![License](https://img.shields.io/badge/license-MIT-yellow.svg?style=flat)](http://yanyiwu.mit-license.org)
[![Build status](https://ci.appveyor.com/api/projects/status/wl30fjnm2rhft6ta/branch/master?svg=true)](https://ci.appveyor.com/project/yanyiwu/cppjieba/branch/master)

[![logo](http://7viirv.com1.z0.glb.clouddn.com/CppJiebaLogo-v1.png)](https://github.com/yanyiwu/cppjieba)

## Introduction

The Jieba Chinese Word Segmentation Implemented By C++ .

## Usage 

### Dependencies

+ `g++ (version >= 4.1 is recommended) or clang++`;
+ `cmake (version >= 2.6 is recommended)`;

### Download & Compile

```sh
git clone --depth=10 --branch=master git://github.com/yanyiwu/cppjieba.git
cd cppjieba
mkdir build
cd build
cmake ..
make
```

### Unit Testing

```
make test
```

## Demo

```
./demo
```

Output:

```
[demo] Cut With HMM
他/来到/了/网易/杭研/大厦
[demo] Cut Without HMM
他/来到/了/网易/杭/研/大厦
我来到北京清华大学
[demo] CutAll
我/来到/北京/清华/清华大学/华大/大学
小明硕士毕业于中国科学院计算所，后在日本京都大学深造
[demo] CutForSearch
小明/硕士/毕业/于/中国/科学/学院/科学院/中国科学院/计算/计算所/，/后/在/日本/京都/大学/日本京都大学/深造
[demo] Insert User Word
男默/女泪
男默女泪
[demo] CutForSearch Word With Offset
[{"word": "小明", "offset": 0}, {"word": "硕士", "offset": 6}, {"word": "毕业", "offset": 12}, {"word": "于", "offset": 18}, {"word": "中国", "offset": 21}, {"word": "科学", "offset": 27}, {"word": "学院", "offset": 30}, {"word": "科学院", "offset": 27}, {"word": "中国科学院", "offset": 21}, {"word": "计算", "offset": 36}, {"word": "计算所", "offset": 36}, {"word": "，", "offset": 45}, {"word": "后", "offset": 48}, {"word": "在", "offset": 51}, {"word": "日本", "offset": 54}, {"word": "京都", "offset": 60}, {"word": "大学", "offset": 66}, {"word": "日本京都大学", "offset": 54}, {"word": "深造", "offset": 72}]
[demo] Tagging
我是拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上CEO，走上人生巅峰。
[我:r, 是:v, 拖拉机:n, 学院:n, 手扶拖拉机:n, 专业:n, 的:uj, 。:x, 不用:v, 多久:m, ，:x, 我:r, 就:d, 会:v, 升职:v, 加薪:nr, ，:x, 当上:t, CEO:eng, ，:x, 走上:v, 人生:n, 巅峰:n, 。:x]
[demo] Keyword Extraction
我是拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上CEO，走上人生巅峰。
[{"word": "CEO", "offset": [93], "weight": 11.7392}, {"word": "升职", "offset": [72], "weight": 10.8562}, {"word": "加薪", "offset": [78], "weight": 10.6426}, {"word": "手扶拖拉机", "offset": [21], "weight": 10.0089}, {"word": "巅峰", "offset": [111], "weight": 9.49396}]
```

Please see details in `test/demo.cpp`.

## Cases

+ [GoJieba] 
+ [NodeJieba]
+ [simhash]
+ [exjieba]
+ [jiebaR]
+ [cjieba]
+ [jieba_rb]
+ [iosjieba]
+ [SqlJieba]
+ [pg_jieba]
+ [ngx_http_cppjieba_module]
+ [gitbook-plugin-search-pro]
+ [cppjieba-server]
+ [perl5-jieba]
+ [jieba-dlang]

## Contact

+ Email: `i@yanyiwu.com`
+ QQ: 64162451
+ WeChat: ![image](http://7viirv.com1.z0.glb.clouddn.com/5a7d1b5c0d_yanyiwu_personal_qrcodes.jpg)

[GoJieba]:https://github.com/yanyiwu/gojieba
[CppJieba]:https://github.com/yanyiwu/cppjieba
[jannson]:https://github.com/jannson
[cppjiebapy]:https://github.com/jannson/cppjiebapy
[cppjiebapy_discussion]:https://github.com/yanyiwu/cppjieba/issues/1
[NodeJieba]:https://github.com/yanyiwu/nodejieba
[jiebaR]:https://github.com/qinwf/jiebaR
[simhash]:https://github.com/yanyiwu/simhash
[exjieba]:https://github.com/falood/exjieba
[cjieba]:http://github.com/yanyiwu/cjieba
[jieba_rb]:https://github.com/altkatz/jieba_rb
[iosjieba]:https://github.com/yanyiwu/iosjieba
[SqlJieba]:https://github.com/yanyiwu/sqljieba
[pg_jieba]:https://github.com/jaiminpan/pg_jieba
[gitbook-plugin-search-pro]:https://plugins.gitbook.com/plugin/search-pro
[cppjieba-server]:https://github.com/yanyiwu/cppjieba-server
[perl5-jieba]:https://metacpan.org/pod/distribution/Lingua-ZH-Jieba/lib/Lingua/ZH/Jieba.pod
[jieba-dlang]:https://github.com/shove70/jieba
