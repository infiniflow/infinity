# CppJieba [English](README_EN.md)

[![CMake](https://github.com/yanyiwu/cppjieba/actions/workflows/cmake.yml/badge.svg)](https://github.com/yanyiwu/cppjieba/actions/workflows/cmake.yml)
[![Author](https://img.shields.io/badge/author-@yanyiwu-blue.svg?style=flat)](http://yanyiwu.com/) 
[![Platform](https://img.shields.io/badge/platform-Linux,%20OS%20X,%20Windows-green.svg?style=flat)](https://github.com/yanyiwu/cppjieba)
[![Performance](https://img.shields.io/badge/performance-excellent-brightgreen.svg?style=flat)](http://yanyiwu.com/work/2015/06/14/jieba-series-performance-test.html) 
[![Tag](https://img.shields.io/github/v/tag/yanyiwu/cppjieba.svg)](https://github.com/yanyiwu/cppjieba/releases)
[![License](https://img.shields.io/badge/license-MIT-yellow.svg?style=flat)](http://yanyiwu.mit-license.org)
[![Build status](https://ci.appveyor.com/api/projects/status/wl30fjnm2rhft6ta/branch/master?svg=true)](https://ci.appveyor.com/project/yanyiwu/cppjieba/branch/master)

## 简介

CppJieba是"结巴(Jieba)"中文分词的C++版本

## 特性

+ 源代码都写进头文件`include/cppjieba/*.hpp`里，`include`即可使用。
+ 支持`utf8`编码。
+ 项目自带较为完善的单元测试，核心功能中文分词(utf8)的稳定性接受过线上环境检验。
+ 支持载自定义用户词典，多路径时支持分隔符'|'或者';'分隔。
+ 支持 `Linux` , `Mac OSX`, `Windows` 操作系统。

## 用法

### 依赖软件

* `g++ (version >= 4.1 is recommended) or clang++`;
* `cmake (version >= 2.6 is recommended)`;

### 下载和编译

```sh
git clone --depth=10 --branch=master git://github.com/yanyiwu/cppjieba.git
cd cppjieba
mkdir build
cd build
cmake ..
make
```

有兴趣的可以跑跑测试(可选):

```
make test
```

## Demo

```
./demo
```

结果示例：

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

详细请看 `test/demo.cpp`.

### 分词结果示例

**MPSegment**

Output:
```
我来到北京清华大学
我/来到/北京/清华大学

他来到了网易杭研大厦
他/来到/了/网易/杭/研/大厦

小明硕士毕业于中国科学院计算所，后在日本京都大学深造
小/明/硕士/毕业/于/中国科学院/计算所/，/后/在/日本京都大学/深造

```

**HMMSegment**

```
我来到北京清华大学
我来/到/北京/清华大学

他来到了网易杭研大厦
他来/到/了/网易/杭/研大厦

小明硕士毕业于中国科学院计算所，后在日本京都大学深造
小明/硕士/毕业于/中国/科学院/计算所/，/后/在/日/本/京/都/大/学/深/造

```

**MixSegment**

```
我来到北京清华大学
我/来到/北京/清华大学

他来到了网易杭研大厦
他/来到/了/网易/杭研/大厦

小明硕士毕业于中国科学院计算所，后在日本京都大学深造
小明/硕士/毕业/于/中国科学院/计算所/，/后/在/日本京都大学/深造

```

**FullSegment**

```
我来到北京清华大学
我/来到/北京/清华/清华大学/华大/大学

他来到了网易杭研大厦
他/来到/了/网易/杭/研/大厦

小明硕士毕业于中国科学院计算所，后在日本京都大学深造
小/明/硕士/毕业/于/中国/中国科学院/科学/科学院/学院/计算/计算所/，/后/在/日本/日本京都大学/京都/京都大学/大学/深造

```

**QuerySegment**

```
我来到北京清华大学
我/来到/北京/清华/清华大学/华大/大学

他来到了网易杭研大厦
他/来到/了/网易/杭研/大厦

小明硕士毕业于中国科学院计算所，后在日本京都大学深造
小明/硕士/毕业/于/中国/中国科学院/科学/科学院/学院/计算所/，/后/在/中国/中国科学院/科学/科学院/学院/日本/日本京都大学/京都/京都大学/大学/深造

```

以上依次是MP,HMM,Mix三种方法的效果。  

可以看出效果最好的是Mix，也就是融合MP和HMM的切词算法。即可以准确切出词典已有的词，又可以切出像"杭研"这样的未登录词。

Full方法切出所有字典里的词语。

Query方法先使用Mix方法切词，对于切出来的较长的词再使用Full方法。

### 自定义用户词典

自定义词典示例请看`dict/user.dict.utf8`。

没有使用自定义用户词典时的结果:

```
令狐冲/是/云/计算/行业/的/专家
```

使用自定义用户词典时的结果:

```
令狐冲/是/云计算/行业/的/专家
```

### 关键词抽取

```
我是拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上CEO，走上人生巅峰。
["CEO:11.7392", "升职:10.8562", "加薪:10.6426", "手扶拖拉机:10.0089", "巅峰:9.49396"]
```

详细请见 `test/demo.cpp`.

### 词性标注

```
我是蓝翔技工拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上总经理，出任CEO，迎娶白富美，走上人生巅峰。
["我:r", "是:v", "拖拉机:n", "学院:n", "手扶拖拉机:n", "专业:n", "的:uj", "。:x", "不用:v", "多久:m", "，:x", "我:r", "就:d", "会:v", "升职:v", "加薪:nr", "，:x", "当上:t", "CEO:eng", "，:x", "走上:v", "人生:n", "巅峰:n", "。:x"]
```

详细请看 `test/demo.cpp`.

支持自定义词性。
比如在(`dict/user.dict.utf8`)增加一行

```
蓝翔 nz
```

结果如下：

```
["我:r", "是:v", "蓝翔:nz", "技工:n", "拖拉机:n", "学院:n", "手扶拖拉机:n", "专业:n", "的:uj", "。:x", "不用:v", "多久:m", "，:x", "我:r", "就:d", "会:v", "升职:v", "加薪:nr", "，:x", "当:t", "上:f", "总经理:n", "，:x", "出任:v", "CEO:eng", "，:x", "迎娶:v", "白富美:x", "，:x", "走上:v", "人生:n", "巅峰:n", "。:x"]
```

## 其它词典资料分享

+ [dict.367W.utf8] iLife(562193561 at qq.com)

## 应用

+ [GoJieba] go语言版本的结巴中文分词。
+ [NodeJieba] Node.js 版本的结巴中文分词。
+ [simhash] 中文文档的的相似度计算
+ [exjieba] Erlang 版本的结巴中文分词。
+ [jiebaR] R语言版本的结巴中文分词。
+ [cjieba] C语言版本的结巴分词。
+ [jieba_rb] Ruby 版本的结巴分词。
+ [iosjieba] iOS 版本的结巴分词。
+ [SqlJieba] MySQL 全文索引的结巴中文分词插件。
+ [pg_jieba] PostgreSQL 数据库的分词插件。
+ [simple] SQLite3 FTS5 数据库的分词插件。
+ [gitbook-plugin-search-pro] 支持中文搜索的 gitbook 插件。
+ [ngx_http_cppjieba_module] Nginx 分词插件。
+ [cppjiebapy] 由 [jannson] 开发的供 python 模块调用的项目 [cppjiebapy], 相关讨论 [cppjiebapy_discussion] .
+ [cppjieba-py] 由 [bung87] 基于 pybind11 封装的 python 模块,使用体验上接近于原jieba。
+ [KeywordServer] 50行搭建一个中文关键词抽取服务。
+ [cppjieba-server] CppJieba HTTP 服务器。
+ [phpjieba] php版本的结巴分词扩展。
+ [perl5-jieba] Perl版本的结巴分词扩展。
+ [jieba-dlang] D 语言的结巴分词 Deimos Bindings。

## 线上演示

[Web-Demo](http://cppjieba-webdemo.herokuapp.com/)
(建议使用chrome打开)

## 性能评测

[Jieba中文分词系列性能评测]

## Sponsorship

[![sponsorship](http://images.gitads.io/cppjieba)](https://tracking.gitads.io/?campaign=gitads&repo=cppjieba&redirect=gitads.io)

## Contributors

### Code Contributors

This project exists thanks to all the people who contribute.
<a href="https://github.com/yanyiwu/cppjieba/graphs/contributors"><img src="https://opencollective.com/cppjieba/contributors.svg?width=890&button=false" /></a>

[GoJieba]:https://github.com/yanyiwu/gojieba
[CppJieba]:https://github.com/yanyiwu/cppjieba
[jannson]:https://github.com/jannson
[cppjiebapy]:https://github.com/jannson/cppjiebapy
[bung87]:https://github.com/bung87
[cppjieba-py]:https://github.com/bung87/cppjieba-py
[cppjiebapy_discussion]:https://github.com/yanyiwu/cppjieba/issues/1
[NodeJieba]:https://github.com/yanyiwu/nodejieba
[jiebaR]:https://github.com/qinwf/jiebaR
[simhash]:https://github.com/yanyiwu/simhash
[代码详解]:https://github.com/yanyiwu/cppjieba/wiki/CppJieba%E4%BB%A3%E7%A0%81%E8%AF%A6%E8%A7%A3
[issue25]:https://github.com/yanyiwu/cppjieba/issues/25
[exjieba]:https://github.com/falood/exjieba
[KeywordServer]:https://github.com/yanyiwu/keyword_server
[ngx_http_cppjieba_module]:https://github.com/yanyiwu/ngx_http_cppjieba_module
[dict.367W.utf8]:https://github.com/qinwf/BigDict
[cjieba]:http://github.com/yanyiwu/cjieba
[jieba_rb]:https://github.com/altkatz/jieba_rb
[iosjieba]:https://github.com/yanyiwu/iosjieba
[SqlJieba]:https://github.com/yanyiwu/sqljieba
[Jieba中文分词系列性能评测]:http://yanyiwu.com/work/2015/06/14/jieba-series-performance-test.html
[pg_jieba]:https://github.com/jaiminpan/pg_jieba
[gitbook-plugin-search-pro]:https://plugins.gitbook.com/plugin/search-pro
[cppjieba-server]:https://github.com/yanyiwu/cppjieba-server
[phpjieba]:https://github.com/jonnywang/phpjieba
[perl5-jieba]:https://metacpan.org/pod/distribution/Lingua-ZH-Jieba/lib/Lingua/ZH/Jieba.pod
[jieba-dlang]:https://github.com/shove70/jieba
[simple]:https://github.com/wangfenjin/simple


