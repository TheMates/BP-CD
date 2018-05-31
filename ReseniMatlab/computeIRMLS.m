function [ IR ] = ComputeIRMLS( output, n )
%Vypoèítá impulsní odezvu z výstupního signálu po MLS
%   INPUT:
%   output = signál z výstupu systému
%   n = øád MLS
%   P1 = permutaèní matice P1 (ve formátu tags) (u mì tags11)
%   P2 = permutaèní matice P2 (ve formátu tags) (u mì tags2)
%
%   OUTPUT
%   IR = impulsní odezva 

%P1 - permutaèní matice ve formátu tags
str = sprintf('tags/tags11(%d).mat',n);
P1 = importdata(str);

%P2 - permutaèní matice ve formátu tags
str = sprintf('tags/tags2(%d).mat',n);
P2 = importdata(str);

Y = output(1:2^n-1);     %oøežu na délku vstupu

%první permutace
y1 = zeros([2^n,1]);
for i=1:(2^n-1)
    y1(i+1) = Y(P1(i));    
end
%provedeme hadamardovu transformaci signálu
Hy1 = HadamTrans(y1);

%druhá permutace
y2 = zeros([2^n-1,1]);
for i=1:(2^n-1)
    y2(i) = Hy1(P2(i)+1);    
end
IR = (1/2^n)*y2;

end

function [ Y ] = HadamTrans( X )
%Funkce pøetransformuje signál jakoby byl vynásobený s Had. maticí, ale
%bude se to dìlat po krocích
%   X = vstupní signál o délce 2^n
lngt = length(X);
n = log2(lngt);

%no jednoduše øeèeno tady ten tripleFOR provádí motýlky, jako je popsanej
%algoritmus FHT. vypadá to složitì, ale jen je to v podstatì práce s
%indexama, která odpovídá napø obrázku 10.4 ze Zpracování akustických
%signálu od Kadlece, nebo figure 7 v 6kapitole v handbooku

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
clear X X1  %to snad ani neni nutný
end

