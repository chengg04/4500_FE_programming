tic
[FileName,PathName] = uigetfile('*.txt','Select the Txt files');
fid=fopen([PathName FileName]); 

fscanf(fid,'%s',1);
n = fscanf(fid,'%d',1);

Q = zeros(n,n);
mu = zeros(n,1);
lowerbound = zeros(n,1);
upperbound = zeros(n,1);
fscanf(fid,'%s',1);
for i = 1:(n*n)
    if mod(i,n) == 0
        col = n;
        row = floor(i/n);
    else
        col = mod(i,n);
        row = floor(i/n)+1;
    end
    Q(row,col) = fscanf(fid,'%f',1);
end

fscanf(fid,'%s',1);

for i = 1:n
    mu(i) = fscanf(fid,'%f',1);
end


for i = 1:n
    fscanf(fid,'%s',1);
    lowerbound(i) = fscanf(fid,'%f',1);
    fscanf(fid,'%s',1);
    upperbound(i) = fscanf(fid,'%f',1);
end

fscanf(fid,'%s',1);
lambda = fscanf(fid,'%f',1);

fclose(fid);  

aeq = ones(1,n);
beq = 1;

[maxX, finalValue] = quadprog(Q,-mu,[],[],aeq,beq,lowerbound,upperbound)

variance = maxX'*Q*maxX
toc