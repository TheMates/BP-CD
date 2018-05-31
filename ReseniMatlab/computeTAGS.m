function [tags1,tags2 ] = ComputeTAGS( mls )
%Dá nám 2 vektory, ve kterejch jsou uložený pozice 1 pro P1 a P2 ale je to
%upravený pro to, aby se nemuselo násobit ètvercovými maticemi P ale jen
%to pøeházet cyklem
%
%optimalizovaný pro dlouhý mls

%pro uložení jména
% i = 10;
% fname = sprintf('myfile%d.mat', i);
% save(fname)



lngt = length(mls);
n = log2(lngt+1);
%pøedìlám ze symetrickýho na binární
mls= int8((mls-1)*(-0.5));


N = zeros([n,lngt],'int8');     % nepotøebuju double,staèí short - int8
%matice N se skládá z postupnì posunutejch signálù mls, pro urèení tags1
%ale potøebuju jen prvních n øádkù
for i = 1:n
    N(i,1:lngt) = circshift(mls,i-1,2);
end


tags1 = zeros([1,lngt]);  %nejdøív urèím velikost, abych pak mohl pozdìji pøièítat k 0
for i = 1:lngt
    for j = 1:n
        tags1(i) = tags1(i)+double(N(j,i))*2^(j-1); 
    end
end

fname = sprintf('tags1(%d).mat',n);
save(fname, 'tags1');

tags11 = zeros([1,lngt]);

%tags1 jsou klíèová data k permutaci sloupcù - ale vstupní signál má jen 1
%sloupec - tudíž budeme chtít permutovat jednotlivé øádky tohoto vektoru, k
%tomu potøebujeme inverzní matici od tags1
for i=1:lngt
    tags11(i) = find(tags1==i);
end
fname = sprintf('tags11(%d).mat',n);
save(fname, 'tags11');




N1 = zeros([lngt,n],'int8');    %nepotøebuju vlastnì celou matici, staèí mi jen n sloupcù ze kterejch poskládám tags2

%tags2 urèuju z 2^n sloupce - 1 2 4 8 16 ... takže si tyhlety sloupce
%pøepíšu do n,lngt pole
for i = 1:n
    ind = find(tags1==(2^(i-1)));
    for j = 1:lngt
       N1(j,i) = mls(mod(ind-j,lngt)+1);  %na základì èísla v tags pøepíšu na pøíslušnej sloupec v N1 sloupec z N  
    end    
end

%teï nepotøebuju, vypustím z pamìti zatím
clear N

tags2 = zeros([1,lngt]);  %nejdøív urèím velikost, abych pak mohl pozdìji pøièítat k 0
for i = 1:lngt
    for j = 1:n
        tags2(i) = tags2(i)+double(N1(i,j))*2^(j-1);
    end
end
clear N1

tags22 = zeros([1,lngt]);
%opìt inverzní matice
for i=1:lngt
    tags22(i) = find(tags2==i);
end
fname = sprintf('tags22(%d).mat',n);
save(fname, 'tags22');
%tags1 jsou klíèová data k permutaci øádkù, jelikož vstupní vektor je
%sloupcový a má opravdu nìkolik øádkù, tak se matice tags2 nemusí nijak
%mìnit, protože je už od zaèátku urèena k permutaci øádkù
fname = sprintf('tags2(%d).mat',n);
save(fname, 'tags2');



end

