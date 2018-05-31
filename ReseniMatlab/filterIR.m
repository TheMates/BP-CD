function [ IRfiltered ] = filterIR( IR,fs )
%filterIR
%Odfiltruje IR od ru�iv�ho nap�t� filtrem HP s mezn�m kmito�tem 100 Hz
%   INPUT:
%   IR = Impulsn� odezva
%
%   OUTPUT:
%   IRfiltered = filtrovan� impulsn� odezvy
HP = designfilt('highpassiir', 'FilterOrder', 8, 'HalfPowerFrequency', ...
                100, 'SampleRate', fs, 'DesignMethod', 'butter');
IRfiltered = filter(HP,IR);
end

