function [ y ] = MovingAverage( IRs,fs,interval,method )
%MovingAverage( x,fs,interval )
%Zpr�m�ruje pr�b�h pohybliv�m pr�m�rovac�m oknem
%   IRs = impulsn� odezva kter� se bude integrovat, JI� UMOCN�N� NA DRUHOU
%   fs = vzrokovac� frekvence
%   interval = d�lka intervalu
%   method: 0 pro zad�v�n� v ms, 1 pro zad�v�n� v po�tu vzork�

if method == 0
    lngt = ceil((fs*interval)/1000);  %zaokrouhlim nahoru

else
    lngt = interval;
end
if rem(lngt,2)      %the number is odd
        lngt = lngt +1;
end
y = smooth(IRs,lngt);
end

