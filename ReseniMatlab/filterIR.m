function [ IRfiltered ] = filterIR( IR,fs )
%filterIR
%Odfiltruje IR od rušivého napìtí filtrem HP s mezním kmitoètem 100 Hz
%   INPUT:
%   IR = Impulsní odezva
%
%   OUTPUT:
%   IRfiltered = filtrovaná impulsní odezvy
HP = designfilt('highpassiir', 'FilterOrder', 8, 'HalfPowerFrequency', ...
                100, 'SampleRate', fs, 'DesignMethod', 'butter');
IRfiltered = filter(HP,IR);
end

