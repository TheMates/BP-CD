function [ env, decay, T60 ] = SchroederIR( IR,log,fs )
%Vypoèítá Schroederùv integrál IR
%   INPUT:
%   IR = Impulsní odezva
%   log = 1 pro logaritmický zobrazení, 0 pro normální
%   fs = vzorkovací kmitoèet
%
%   OUTPUT:
%   env = prùbìh Schroedrova integrálu
%   decay = koeficienty pro pøímku poklesu
%   T60 = odhad doby dozvuku

env = flipud(cumsum(flipud(abs(IR))));
if log == 1
    env = 10*log10(env/max(env));
end

lngt = ceil(length(env)*0.75);      %budu používat jen 3/4 z poklesu, 
%protože ten zadek je vìtšinou nepoužitelnej a pokud budu integrovat 
%do crosspointu z Lundebyho, tak by mìly bejt 3/4 v pohodì

dynrange = max(env(1:lngt))-min(env(1:lngt));
dbstartmulti = 0.1; %od kdy dìlám výpoèet regrese
dbendmulti = 0.75; %do kdy dìlám výpoèet regrese

dbstart = -dbstartmulti*dynrange;   %hodnota od kdy budu poèítat regresi
dbend = -dbendmulti*dynrange;       %hodnota do kdy budu poèítat regresi

tdecstart = find(env<dbstart,1,'first');    %index od kdy
tdecstop = find(env<dbend,1,'first');       %index do kdy

x = tdecstart:1:tdecstop;
y = env(tdecstart:tdecstop);
p = polyfit(x.',y,1);                       %tohle mi hodí koeficienty pro pøímku regrese

decay = p;
deltax = -60/decay(1);      %kolik samplù na pokles o 60 dB
T60 = deltax/fs;

end

