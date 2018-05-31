function [tags1,tags2 ] = ComputeTAGS( mls )
%D� n�m 2 vektory, ve kterejch jsou ulo�en� pozice 1 pro P1 a P2 ale je to
%upraven� pro to, aby se nemuselo n�sobit �tvercov�mi maticemi P ale jen
%to p�eh�zet cyklem
%
%optimalizovan� pro dlouh� mls

%pro ulo�en� jm�na
% i = 10;
% fname = sprintf('myfile%d.mat', i);
% save(fname)



lngt = length(mls);
n = log2(lngt+1);
%p�ed�l�m ze symetrick�ho na bin�rn�
mls= int8((mls-1)*(-0.5));


N = zeros([n,lngt],'int8');     % nepot�ebuju double,sta�� short - int8
%matice N se skl�d� z postupn� posunutejch sign�l� mls, pro ur�en� tags1
%ale pot�ebuju jen prvn�ch n ��dk�
for i = 1:n
    N(i,1:lngt) = circshift(mls,i-1,2);
end


tags1 = zeros([1,lngt]);  %nejd��v ur��m velikost, abych pak mohl pozd�ji p�i��tat k 0
for i = 1:lngt
    for j = 1:n
        tags1(i) = tags1(i)+double(N(j,i))*2^(j-1); 
    end
end

fname = sprintf('tags1(%d).mat',n);
save(fname, 'tags1');

tags11 = zeros([1,lngt]);

%tags1 jsou kl��ov� data k permutaci sloupc� - ale vstupn� sign�l m� jen 1
%sloupec - tud� budeme cht�t permutovat jednotliv� ��dky tohoto vektoru, k
%tomu pot�ebujeme inverzn� matici od tags1
for i=1:lngt
    tags11(i) = find(tags1==i);
end
fname = sprintf('tags11(%d).mat',n);
save(fname, 'tags11');




N1 = zeros([lngt,n],'int8');    %nepot�ebuju vlastn� celou matici, sta�� mi jen n sloupc� ze kterejch poskl�d�m tags2

%tags2 ur�uju z 2^n sloupce - 1 2 4 8 16 ... tak�e si tyhlety sloupce
%p�ep�u do n,lngt pole
for i = 1:n
    ind = find(tags1==(2^(i-1)));
    for j = 1:lngt
       N1(j,i) = mls(mod(ind-j,lngt)+1);  %na z�klad� ��sla v tags p�ep�u na p��slu�nej sloupec v N1 sloupec z N  
    end    
end

%te� nepot�ebuju, vypust�m z pam�ti zat�m
clear N

tags2 = zeros([1,lngt]);  %nejd��v ur��m velikost, abych pak mohl pozd�ji p�i��tat k 0
for i = 1:lngt
    for j = 1:n
        tags2(i) = tags2(i)+double(N1(i,j))*2^(j-1);
    end
end
clear N1

tags22 = zeros([1,lngt]);
%op�t inverzn� matice
for i=1:lngt
    tags22(i) = find(tags2==i);
end
fname = sprintf('tags22(%d).mat',n);
save(fname, 'tags22');
%tags1 jsou kl��ov� data k permutaci ��dk�, jeliko� vstupn� vektor je
%sloupcov� a m� opravdu n�kolik ��dk�, tak se matice tags2 nemus� nijak
%m�nit, proto�e je u� od za��tku ur�ena k permutaci ��dk�
fname = sprintf('tags2(%d).mat',n);
save(fname, 'tags2');



end

