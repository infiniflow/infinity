Performance
===========

to be written


Data size comparison
====================

The directory [*tests/jsonSample*](https://github.com/arangodb/velocypack/tree/main/tests/jsonSample)
contains several example JSON files which can be converted to VPack values. 
The following section shows how much can be gained by converting these JSON 
data into VPack values.

The resulting VPack sizes are compared against some other popular schema-free 
serialization formats such as [MessagePack / MsgPack](http://msgpack.org/)
and [BSON](http://bsonspec.org/).

The following table contains the size comparisons for the beforementioned 
files and formats. All values in the table are byte sizes. Smaller values are
better. 

The columns in the table have the following meanings:

* *original JSON*: size of the original JSON data (potentially including whitespace)
* *net JSON*: size of the remaining JSON data after stripping whitespace
* *BSON*: size after converting JSON input to BSON
* *MsgPack*: size after converting JSON input to MsgPack
* *VPack*: size after converting JSON input to VPack
* *VPack-c*: size after converting JSON input to VPack, using *compact* option in VPack
* *VPc-gzip*: size after running `gzip -9` on the VPack-c output
* *VPc-snappy*: size after using snappy-compression on the VPack-c output

```
test case         original JSON    net JSON     MsgPack        BSON       VPack     VPack-c  VPc-gzip VPc-snappy
----------------------------------------------------------------------------------------------------------------
api-docs.json         1,205,964   1,054,928   1,003,337   1,012,938   1,004,012     994,160   101,954    188,967
commits.json             25,216      25,216      23,091      30,907      22,935      20,789     3,863      5,617 
countries.json        1,134,029   1,134,029   1,161,461   1,367,765   1,061,291     956,786   230,890    350,236 
directory-tree.json     297,695     297,695     306,534     368,518     276,862     244,716    67,015     94,354 
doubles.json          1,187,062   1,187,062     899,981   1,488,895   1,299,984     899,982   418,291    837,753 
doubles-small.json      158,706     108,705      89,995     138,895     130,001      89,998    52,321     77,465  
file-list.json          151,317     151,316     139,979     195,268     150,177     133,536    42,463     62,585
object.json             157,781     157,781     118,519     178,895     158,633     118,630    30,396     61,269
pass1.json                1,441         961         844       1,181         916         804       687        822
pass2.json                   52          52          32         169          51          51        88         69
pass3.json                  148         115         124         123         110         108       132        126
random1.json              9,672       7,577       7,007       8,209       7,310       6,836     2,931      3,762
random2.json              8,239       6,443       5,960       6,992       6,222       5,815     2,520      3,249
random3.json             72,953      57,112      52,780      61,903      55,066      51,515    17,781     24,513
sample.json             687,491     168,089     150,697     155,462     162,113     153,187    22,891     25,300
small.json                   82          58          35          79          35          30        67         48
```

Data size comparison, with Object key compression
=================================================

Object keys can optionally be compressed in the VPack format.
Object key compression works by storing a (small) integer value instead of the original 
(potentially much longer) object attribute name. Especially when attribute names are long,
this compression can save lots of storage space. Attribute name translation is also fast
as attribute names and their integer replacement values are maintained in a hash table
which has O(1) lookup performance.

The compression may or may not be suitable for all use cases. VelocyPack data is schema-free,
so arbitrary types of objects can be stored with it. However, in many use cases many objects
share a common set of attribute names. In the most extreme use case, all objects even have the
same attribute names. If this is known in advance, the object key compression makes sense.

The table coming up shows the resulting VPack sizes when the Object key compression is
turned on. For all cases in the table, the object keys of the input data were first analyzed
with an extra pass over the data. This pass collected all key names and counted the number
of occurrences. After that pass, the keys that occur multiple times in the input data were
put into a dictionary. This dictionary was then used when converting the JSON input to VPack
regularly in the second pass.

In some real-world use cases, two passes over the data may not be desired for performance
reasons. If the application has some idea about the data it will store in VPack, it can still
use the compression for the most frequently occurring attribute names it knows about. In this
case, the dictionary will probably be populated once at application start. This static dictionary 
can then be used when building all following VPack objects. This removes the need for an extra
inspection pass over the input data, but still will provide some compression benefits.

If an application has absolutely no idea about what kind of data it will store in VPack and 
there is no common key translation dictionary, an application can still build a per-VPack
dictionary. In this case there is no alternative to making two passes over the data, and
storing the dictionary somewhere with the generated VPack dictionary. This will not be highly
efficient, but still can reduce the resulting VPack object sizes massively.

The following table shows the original VPack and other format sizes as in the previous table,
but adds two extra columns with Object key compression turned on. The column description can be
found below.

```
test case         original JSON    net JSON     MsgPack        BSON     VPack-c   |   VPack-d      Dict
----------------------------------------------------------------------------------|--------------------
api-docs.json         1,205,964   1,054,928   1,003,337   1,012,938     994,160   |   964,647     1,091
commits.json             25,216      25,216      23,091      30,907      20,789   |     9,200       983
countries.json        1,134,029   1,134,029   1,161,461   1,367,765     956,786   |   574,333        47 
directory-tree.json     297,695     297,695     306,534     368,518     244,716   |   194,391       881 
doubles.json          1,187,062   1,187,062     899,981   1,488,895     899,982   |   899,982         0 
doubles-small.json      158,706     108,705      89,995     138,895      89,998   |    89,998         0
file-list.json          151,317     151,316     139,979     195,268     133,536   |   124,849       872
object.json             157,781     157,781     118,519     178,895     118,630   |   118,630         0
pass1.json                1,441         961         844       1,181         804   |       804         0
pass2.json                   52          52          32         169          51   |        51         0
pass3.json                  148         115         124         123         108   |       108         0
random1.json              9,672       7,577       7,007       8,209       6,836   |     5,807       231
random2.json              8,239       6,443       5,960       6,992       5,815   |     4,933       231
random3.json             72,953      57,112      52,780      61,903      51,515   |    43,724       231
sample.json             687,491     168,089     150,697     155,462     153,187   |    48,977    56,679
small.json                   82          58          35          79          35   |        30         0
```

The columns in the above table have the following meanings:

* *original JSON*: size of the original JSON data (potentially including whitespace)
* *net JSON*: size of the remaining JSON data after stripping whitespace
* *BSON*: size after converting JSON input to BSON
* *MsgPack*: size after converting JSON input to MsgPack
* *VPack-c*: size after converting JSON input to VPack, using *compact* option in VPack
* *VPack-d*: size after converting JSON input to VPack, using *compact* option and object key compression
* *Dict*: size of the generated object key dictionary 

All values are byte sizes.

Note that when the dictionary size is 0, there would be no benefit from compressing object keys,
likely because the input data does not contain repeating attribute key names or not even objects.

Still for most inputs the object key compression makes sense space-wise, even when taking the size of
the object key dictionary into account. In most cases, the combined size of the compressed VPack value
and the dictionary are way smaller than when not using the compression.

Converting other JSON files to VPack
====================================

The VPack library contains the tool binary `json-to-vpack`. It can be used to convert any JSON
input to a VPack value. It provides the options `--compress` and `--no-compress` for toggling the
Object key compression, and the options `--compact` and `--no-compact` for toggling the usage of
the compact format.

To convert the input file `input.json` to a VPack value file `output.vpack` while using both the
compact format and the object key compression, use this command:

`json-to-vpack --compact --compress input.json output.vpack`

Please have a look at the [tools README](tools/README.md) for further details on the tools
shipped with the VPack library.
