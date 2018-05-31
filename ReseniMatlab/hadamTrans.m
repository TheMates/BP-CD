function [ Y ] = HadamTrans( X )
%Funkce p�etransformuje sign�l jakoby byl vyn�soben� s Had. matic�, ale
%bude se to d�lat po kroc�ch
%   X = vstupn� sign�l o d�lce 2^n
lngt = length(X);
n = log2(lngt);

%no jednodu�e �e�eno tady ten tripleFOR prov�d� mot�lky, jako je popsanej
%algoritmus FHT. vypad� to slo�it�, ale jen je to v podstat� pr�ce s
%indexama, kter� odpov�d� nap� obr�zku 10.4 ze Zpracov�n� akustick�ch
%sign�lu od Kadlece, nebo figure 7 v 6kapitole v Handbook of signal
%processing in acoustics od Havelocka aj.

%uchov�v�m stavovou prom�nnou X1, kter� je v�sledkem jednotliv� f�ze
%mot�lk�

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

