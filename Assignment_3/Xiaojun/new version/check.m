load('dump2.csv')

p1 = dump2(:,1:(end-1));
p2 = dump2(:,2:end);
r = (p2 - p1) ./ p1;
r = r';
Q = cov(r);

[a , b ] = eig(Q);

x = b(end - 9 : end,end - 9 : end)
y = a(1:10,end - 9 : end)



