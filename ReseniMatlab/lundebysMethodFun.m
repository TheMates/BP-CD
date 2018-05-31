function [ env, tdecstart, decay, noise, crosspoint, T60] = LundebysMethodFun( IR, fs )
%Vypo��t� odhad doby dozvuku, �rovn� �umu.
%function [ env, predelay, decay, noise, crosspoint ] = LundelbysMethodFun( IR, fs )
%   INPUT:
%   IR = impulsn� odezva
%   fs = vzorkovac� fekvence
%
%   OUTPUT:
%   env = ob�lka kter� je po ��stech integrovan� v dB
%   tdecstart = za��tek v�po�tu poklesov� p��mky
%   decay = parametry p��mky kter� kop�ruje decay y = k*x +q, decay(1) = k, decay(2) = q
%   noise = �rove� hluku pozad� v dB
%   crosspoint = bod, od kdy se integruje hladina hluku do konce odezvy
%   T60 = doba poklesu, po��t� se z p��mky regrese

%podle Estimation of Modal Decay parameters from Noisy Response Measuremens
% - karjalainen, poju atd
%2.5.2 Lundeby's Method

%% tyhlety parametry by mohly bejt nastaviteln�, bu� takhle dan� jako auto, nebo pak si je ka�dej m��e nastavit s�m

dbstartmulti = 0.1; %od kdy d�l�m v�po�et regrese
dbupmulti = 0.25; %do kdy d�l�m v�po�et regrese
dbdownmulti = 0.2;  %o kolik dol� budu posouvat crosspoint

ms = 30;
%%
IRs = IR.^2;    %IR nadruhou
% env =log_10(IRs);   %p�evedu na log m�ru a max bude na 0
%% 1)

env = movingAverage(IRs,fs,ms,0);   %po ��stech integruju s defaultn� po��te�n� hodnotou intervalu 30 ms
env = log_10(env);

dynrange = abs((max(env)-min(env)));    %ur��m si dynamick� rozsah IR jako rozd�l nejvy��� a nejni��� hodnoty z integrovan� IR
%% podle toho nastav�m parametry
dbup = dbupmulti*dynrange;  %DO kdy se bude po��tat decay
dbstart = -dbstartmulti*dynrange;   %OD kdy se bude po��tat decay
dbdown = dbdownmulti*dynrange;  %posun pro crosspoint (kde se protne decay p��mka s touto hodnotou, bude nov� crossponint) 

%% 2)
%za�nu t�eba na 50% d�lky 
t1 = ceil(length(IRs)/2);
noise = mean(env(t1:length(env)));

%% 3)
[M,predelay] = max(env);   %najdu nejvy��� hodnotu
tdecstop = find(env(predelay:length(env))<dbup+noise,1,'first')+predelay;  %posledn� hodnota, kde je envelope vy��� ne� hladina dbup nad �rovn� �umu (m��e se st�t, �e nastane chyba, kdyby byl v oblasti hluku n�jak� peak, ale to redukuje i integrace)
x = predelay:1:tdecstop;
y = env(predelay:tdecstop);    
p = polyfit(x.',y,1);   %koeficienty obsahuj� y = k*x + y ; k = p(1), q = p(2)
yy = p(1).*x + p(2);    %decay p��mka  
x1 = predelay:1:length(IRs);
yy1 = p(1).*x1 + p(2);  %decay p��mka i mimo body tdecstart (to je predelay), tdecstop

%% 5)
deltax = -dynrange/p(1);% y=k*x dy = k*dx - vypo��t�m kolik vzork� x ub�hne ne� se zm�n� hodnota z nejvy��� na nejn힚� 
                        %(kolik vzork�, hled�m-li na p��mku yy)
newpocet = 30;          %a ten rozd�l�m na 30 vzrok� - pro 60dB to bude 5 interval� na 10dB ale p�i extr�mn� n�zkejch 
                        %SNR to bude t�eba interval pro 0,2dB poklesu
newinterval = ceil(deltax/newpocet);    %novej interval pro integraci, nahrad� p�vodn� prom�nnou ms

%% 6)
env = movingAverage(IRs,fs,newinterval,1);
env = log_10(env);
[M,predelay] = max(env);   %znovu najdu po��tek

newcrosspointtemp = []; %alokuju z�lohu crosspoitu
for i = 1:3 %zde provedu 3 iterace, podle reference sta�� maxim�ln� 5, p�i posledn�ch 2 z 3 se to u� fakt skoro nezm�n�
%% 4)

%tohle o�et�uje to, aby v�dycky byl n�jakej crosspoint, kdybych u� nena�el
%hodnotu kter� je od p�edchoz�ho noise flooru ni��� o dbdown tak crosspoint
%posunu o polovinu bl� ke konci sekvence
newcrosspoint = find(yy1<noise-dbdown,1,'first')+ x1(1); %yy1 je ten pokles ale v t� prom�nn� je to zapsan� od prnv�ho indexu ten pokles
%a plotuje se to v z�vislosti na x1 kter� to posune do doby, kde opravdu
%ten pokles je, tak�e mus�m p�i��st prvn� prvek x1 proto�e ten n�m
%�ekne,kde ten crosspoint OPRAVDU je
if isempty(newcrosspoint) == true
    newcrosspoint = newcrosspointtemp+((length(IRs)-newcrosspointtemp)/2);
else
    newcrosspointtemp = newcrosspoint;
end

noise = mean(env(newcrosspoint:length(env)));  %ur��m hladinu hluku od nov�ho crosspointu
%% 7) new crosspoint
tdecstart = find(env>dbstart,1,'last');     %zde u� se po��t� ne od za��tku jako v sekci 3) ale od ni��� hodnoty ne� je max, nastaven� podle dbstart
tdecstop = find(env(tdecstart:length(env))<dbup+noise,1,'first')+tdecstart;
x = tdecstart:1:tdecstop;
y = env(tdecstart:tdecstop);
p = polyfit(x.',y,1);
yy = p(1).*x + p(2);
x1 = predelay:1:length(IRs);
yy1 = p(1).*x1 + p(2);
end

decay = p;
crosspoint = newcrosspoint;
deltax = -60/decay(1);      %kolik sampl� na pokles o 60 dB
T60 = deltax/fs;

end

