function [ env, decay, T60 ] = SchroederIR( IR,log,fs )
%Vypo��t� Schroeder�v integr�l IR
%   INPUT:
%   IR = Impulsn� odezva
%   log = 1 pro logaritmick� zobrazen�, 0 pro norm�ln�
%   fs = vzorkovac� kmito�et
%
%   OUTPUT:
%   env = pr�b�h Schroedrova integr�lu
%   decay = koeficienty pro p��mku poklesu
%   T60 = odhad doby dozvuku

env = flipud(cumsum(flipud(abs(IR))));
if log == 1
    env = 10*log10(env/max(env));
end

lngt = ceil(length(env)*0.75);      %budu pou��vat jen 3/4 z poklesu, 
%proto�e ten zadek je v�t�inou nepou�itelnej a pokud budu integrovat 
%do crosspointu z Lundebyho, tak by m�ly bejt 3/4 v pohod�

dynrange = max(env(1:lngt))-min(env(1:lngt));
dbstartmulti = 0.1; %od kdy d�l�m v�po�et regrese
dbendmulti = 0.75; %do kdy d�l�m v�po�et regrese

dbstart = -dbstartmulti*dynrange;   %hodnota od kdy budu po��tat regresi
dbend = -dbendmulti*dynrange;       %hodnota do kdy budu po��tat regresi

tdecstart = find(env<dbstart,1,'first');    %index od kdy
tdecstop = find(env<dbend,1,'first');       %index do kdy

x = tdecstart:1:tdecstop;
y = env(tdecstart:tdecstop);
p = polyfit(x.',y,1);                       %tohle mi hod� koeficienty pro p��mku regrese

decay = p;
deltax = -60/decay(1);      %kolik sampl� na pokles o 60 dB
T60 = deltax/fs;

end

