function [ y ] = MovingAverage( IRs,fs,interval,method )
%MovingAverage( x,fs,interval )
%Zprùmìruje prùbìh pohyblivým prùmìrovacím oknem
%   IRs = impulsní odezva která se bude integrovat, JIŽ UMOCNÌNÁ NA DRUHOU
%   fs = vzrokovací frekvence
%   interval = délka intervalu
%   method: 0 pro zadávání v ms, 1 pro zadávání v poètu vzorkù

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

