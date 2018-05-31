function [ Y ] = HadamTrans( X )
%Funkce pøetransformuje signál jakoby byl vynásobený s Had. maticí, ale
%bude se to dìlat po krocích
%   X = vstupní signál o délce 2^n
lngt = length(X);
n = log2(lngt);

%no jednoduše øeèeno tady ten tripleFOR provádí motýlky, jako je popsanej
%algoritmus FHT. vypadá to složitì, ale jen je to v podstatì práce s
%indexama, která odpovídá napø obrázku 10.4 ze Zpracování akustických
%signálu od Kadlece, nebo figure 7 v 6kapitole v Handbook of signal
%processing in acoustics od Havelocka aj.

%uchovávám stavovou promìnnou X1, která je výsledkem jednotlivé fáze
%motýlkù

X1 = zeros([lngt,1]);
for j=1:n
    for i=1:2^j:(lngt-j)
        for k=1:2^(j-1)   
            X1(i+(k-1)) = X(i+(k-1))+X(i+2^(j-1)+(k-1));
            X1(i+2^(j-1)+(k-1))=X(i+(k-1))-X(i+2^(j-1)+(k-1));
        end
    end
    X = X1;
end
Y = X1;
clear X X1
end

