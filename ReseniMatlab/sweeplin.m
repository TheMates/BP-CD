function [ y ] = sweeplin( f1,f2,t,fs )
%Funkce kter� vygeneruje line�rn� p�ela�ovan� harmonick� sign�l%   
%   f1 = po��te�n� frekvence [Hz]
%   f2 = koncov� frekvence [Hz]
%   t = d�lka [ms]
%   fs = vzorkovac� frekvence [Hz]
N = fs*t/1000;
fi = 0;
deltafi = 2*pi*(f1/fs);
Incfi = 2*pi*((f2-f1)/(fs*N));

y = 1:N;
for i=1:(N)
    y(i) = sin(fi);
    fi = fi + deltafi;
    deltafi = deltafi + Incfi;
end
end

