1. f(u) = [min(g(u), rhs(u)+h(u)+km), min(g(u), rhs(u))]

2. Initialize:
3.  Q.insert(goal, f(goal))
4.  rhs(S), g(S) = inf
5.  rhs(goal) = 0

6. UpdateVertex(u):
7.  if(g(u)!=rhs(u))
8.     U.Update(u, f(u))
9.   else if(g(u) != rhs(u))
10.      U.insert(u, f(u))
11.    else
12.      U.remove(u)

13.  ComputeShorthestPath:
14.    while( U.topKey < f(start) or rhs(start) > g(start) )
15.      u = U.top
16.      k_old = U.topKey
17.      if(k_old < f(u))
18.        U.update(u, f(u));
19.      else if(g(u) > rhs(u))
20.        g(u) = rhs(u)
21.        U.remove(u)
22.        for each s in nbrs(u)
23.          if(s != goal)
24.            rhs(s) = min(rhs(s), g(u) + 1)
25.            UpdateVertex(s)
26.      else  
27.        g_old = g(u)
28.        g(u) = inf
29.        for each s in nbrs(u)&{u}
30.          if(rhs(s) == g_old+1 and s != goal)
31.            rhs(s) = min of g(nbrs(s))+1
32.          UpdateVertex(s)
