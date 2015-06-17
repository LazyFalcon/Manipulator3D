1 . f(x) = [min(h(x), k(x)+g(x)+km), min(h(x), k(x))]

2 . Insert(h, h_new):
3 .   if( t(x)==NEW )
4 .     k(x) = h_new
5 .   else if( t(x)==OPEN )
6 .     k(x) = min( k(x), h_new )
7 .     U.remove(x)
8 .   else
9 .     k(x) = min(h(x), h_new)
10.   h(x) = h_new
11.   U.insert(x, f(x))

12. ComputeShorthestPath:
13.   while
14.     x = U.top
15.     val = f(x)
16.     U.remove(x)
17.     if( k(x)<h(x) )
18.       for each y in nbrs(x)
19.         if(t(y)!=NEW and cost(y)<=val and k(x)==k(y)+1)
20.           h(x) = h(y)+1
21.           b(x) = y
22.     if(k(x)==h(x))
23.       for each y in nbrs(x)
24.         if( t(y)==NEW or (x==b(y) and h(y)!=h(x)+1) or (x!=b(y) and h(y)>h(x)+1) )
25.           insert(y, h(x)+1)
26.           b(y) = x
27.     else  
28.       for each y in nbrs(x)
29.         if( t(y)==NEW or h(y)!=h(x)+1)
30.           insert(y, h(x)+1)
31.           b(y) = x
32.         else  
33.           if( b(y)!=x and h(y)>h(x)+1 and t(x)==CLOSED)
34.             insert(x, h(x))
35.           else if( b(y)!=x and h(x)>h(y)+1 and t(y)==CLOSED and f(y)<val )
36.             insert(x, h(x))
