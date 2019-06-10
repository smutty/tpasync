# tp_async

C++ thread pool based async exercise
## test_quicksort output

Hardware threads: 4

```Seq quicksort 		400000 items |	time elapsed (ms): 5292
std::async quicksort 	400000 items |	time elapsed (ms): 5677
tp::async quicksort 	400000 items |	time elapsed (ms): 3222
-----------------
Seq quicksort 		400000 items |	time elapsed (ms): 8155
std::async quicksort 	400000 items |	time elapsed (ms): 8693
tp::async quicksort 	400000 items |	time elapsed (ms): 6641
-----------------
Seq quicksort 		400000 items |	time elapsed (ms): 8066
std::async quicksort 	400000 items |	time elapsed (ms): 6614
tp::async quicksort 	400000 items |	time elapsed (ms): 4421
-----------------
Seq quicksort 		400000 items |	time elapsed (ms): 8173
std::async quicksort 	400000 items |	time elapsed (ms): 8048
tp::async quicksort 	400000 items |	time elapsed (ms): 5265
-----------------
Seq quicksort 		400000 items |	time elapsed (ms): 8069
std::async quicksort 	400000 items |	time elapsed (ms): 7404
tp::async quicksort 	400000 items |	time elapsed (ms): 2511
-----------------
Seq quicksort 		400000 items |	time elapsed (ms): 8088
std::async quicksort 	400000 items |	time elapsed (ms): 8235
tp::async quicksort 	400000 items |	time elapsed (ms): 2580
-----------------
Seq quicksort 		400000 items |	time elapsed (ms): 7980
std::async quicksort 	400000 items |	time elapsed (ms): 7374
tp::async quicksort 	400000 items |	time elapsed (ms): 5218
-----------------
Seq quicksort 		400000 items |	time elapsed (ms): 7928
std::async quicksort 	400000 items |	time elapsed (ms): 7892
tp::async quicksort 	400000 items |	time elapsed (ms): 4711
-----------------
Seq quicksort 		400000 items |	time elapsed (ms): 7859
std::async quicksort 	400000 items |	time elapsed (ms): 7255
tp::async quicksort 	400000 items |	time elapsed (ms): 6175
-----------------
Seq quicksort 		400000 items |	time elapsed (ms): 8241
std::async quicksort 	400000 items |	time elapsed (ms): 6812
tp::async quicksort 	400000 items |	time elapsed (ms): 5370
-----------------


## test_async output
std::async time elapsed (ms): 1687
tp::async time elapsed (ms): 65
```