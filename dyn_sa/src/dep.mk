build_time.o: build_time.cpp dynSA.h types.h gRankS.h sbvtree.h rbtree.h \
  intTree.h customTree.h lcp.h DSASampling.h lpermutation.h linkedTree.h
build_timeSA.o: build_timeSA.cpp dynSA.h types.h gRankS.h sbvtree.h \
  rbtree.h intTree.h customTree.h lcp.h DSASampling.h lpermutation.h \
  linkedTree.h sarray.h
customTree.o: customTree.cpp customTree.h types.h rbtree.h
displaySA.o: displaySA.cpp sarray.h
DSASampling.o: DSASampling.cpp DSASampling.h lpermutation.h linkedTree.h \
  customTree.h types.h rbtree.h sbvtree.h dynSA.h gRankS.h intTree.h \
  lcp.h
dynSA.o: dynSA.cpp dynSA.h types.h gRankS.h sbvtree.h rbtree.h intTree.h \
  customTree.h lcp.h DSASampling.h lpermutation.h linkedTree.h
gRankS.o: gRankS.cpp gRankS.h types.h sbvtree.h rbtree.h
insert.o: insert.cpp dynSA.h types.h gRankS.h sbvtree.h rbtree.h \
  intTree.h customTree.h lcp.h DSASampling.h lpermutation.h linkedTree.h
intTree.o: intTree.cpp intTree.h customTree.h types.h rbtree.h
lcp.o: lcp.cpp lcp.h dynSA.h types.h gRankS.h sbvtree.h rbtree.h \
  intTree.h customTree.h DSASampling.h lpermutation.h linkedTree.h \
  utils.h
lpermutation.o: lpermutation.cpp lpermutation.h linkedTree.h customTree.h \
  types.h rbtree.h
modifSA.o: modifSA.cpp dynSA.h types.h gRankS.h sbvtree.h rbtree.h \
  intTree.h customTree.h lcp.h DSASampling.h lpermutation.h linkedTree.h \
  sarray.h
npermutation.o: npermutation.cpp npermutation.h intTree.h customTree.h \
  types.h rbtree.h
permutation.o: permutation.cpp permutation.h sbvtree.h rbtree.h types.h
rbtree.o: rbtree.cpp rbtree.h types.h
sbvtree.o: sbvtree.cpp sbvtree.h rbtree.h types.h
slidingWindow.o: slidingWindow.cpp dynSA.h types.h gRankS.h sbvtree.h \
  rbtree.h intTree.h customTree.h lcp.h DSASampling.h lpermutation.h \
  linkedTree.h utils.h
ssatree.o: ssatree.cpp ssatree.h rbtree.h types.h
test.o: test.cpp dynSA.h types.h gRankS.h sbvtree.h rbtree.h intTree.h \
  customTree.h lcp.h DSASampling.h lpermutation.h linkedTree.h utils.h
testgRankS.o: testgRankS.cpp dynSA.h types.h gRankS.h sbvtree.h rbtree.h \
  intTree.h customTree.h lcp.h DSASampling.h lpermutation.h linkedTree.h \
  utils.h
utils.o: utils.cpp utils.h types.h
sarray.o: sarray.c sarray.h
scode.o: scode.c sarray.h
