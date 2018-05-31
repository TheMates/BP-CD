function [ IR ] = ComputeIRMLS( output, n )
%Vypo��t� impulsn� odezvu z v�stupn�ho sign�lu po MLS
%   INPUT:
%   output = sign�l z v�stupu syst�mu
%   n = ��d MLS
%   P1 = permuta�n� matice P1 (ve form�tu tags) (u m� tags11)
%   P2 = permuta�n� matice P2 (ve form�tu tags) (u m� tags2)
%
%   OUTPUT
%   IR = impulsn� odezva 

%P1 - permuta�n� matice ve form�tu tags
str = sprintf('tags/tags11(%d).mat',n);
P1 = importdata(str);

%P2 - permuta�n� matice ve form�tu tags
str = sprintf('tags/tags2(%d).mat',n);
P2 = importdata(str);

Y = output(1:2^n-1);     %o�e�u na d�lku vstupu

%prvn� permutace
y1 = zeros([2^n,1]);
for i=1:(2^n-1)
    y1(i+1) = Y(P1(i));    
end
%provedeme hadamardovu transformaci sign�lu
Hy1 = HadamTrans(y1);

%druh� permutace
y2 = zeros([2^n-1,1]);
for i=1:(2^n-1)
    y2(i) = Hy1(P2(i)+1);    
end
IR = (1/2^n)*y2;

end

function [ Y ] = HadamTrans( X )
%Funkce p�etransformuje sign�l jakoby byl vyn�soben� s Had. matic�, ale
%bude se to d�lat po kroc�ch
%   X = vstupn� sign�l o d�lce 2^n
lngt = length(X);
n = log2(lngt);

%no jednodu�e �e�eno tady ten tripleFOR prov�d� mot�lky, jako je popsanej
%algoritmus FHT. vypad� to slo�it�, ale jen je to v podstat� pr�ce s
%indexama, kter� odpov�d� nap� obr�zku 10.4 ze Zpracov�n� akustick�ch
%sign�lu od Kadlece, nebo figure 7 v 6kapitole v handbooku

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
clear X X1  %to snad ani neni nutn�
end

