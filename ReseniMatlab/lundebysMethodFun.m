function [ env, tdecstart, decay, noise, crosspoint, T60] = LundebysMethodFun( IR, fs )
%Vypoèítá odhad doby dozvuku, úrovnì šumu.
%function [ env, predelay, decay, noise, crosspoint ] = LundelbysMethodFun( IR, fs )
%   INPUT:
%   IR = impulsní odezva
%   fs = vzorkovací fekvence
%
%   OUTPUT:
%   env = obálka která je po èástech integrovaná v dB
%   tdecstart = zaèátek výpoètu poklesové pøímky
%   decay = parametry pøímky která kopíruje decay y = k*x +q, decay(1) = k, decay(2) = q
%   noise = úroveò hluku pozadí v dB
%   crosspoint = bod, od kdy se integruje hladina hluku do konce odezvy
%   T60 = doba poklesu, poèítá se z pøímky regrese

%podle Estimation of Modal Decay parameters from Noisy Response Measuremens
% - karjalainen, poju atd
%2.5.2 Lundeby's Method

%% tyhlety parametry by mohly bejt nastavitelný, buï takhle daný jako auto, nebo pak si je každej mùže nastavit sám

dbstartmulti = 0.1; %od kdy dìlám výpoèet regrese
dbupmulti = 0.25; %do kdy dìlám výpoèet regrese
dbdownmulti = 0.2;  %o kolik dolù budu posouvat crosspoint

ms = 30;
%%
IRs = IR.^2;    %IR nadruhou
% env =log_10(IRs);   %pøevedu na log míru a max bude na 0
%% 1)

env = movingAverage(IRs,fs,ms,0);   %po èástech integruju s defaultní poèáteèní hodnotou intervalu 30 ms
env = log_10(env);

dynrange = abs((max(env)-min(env)));    %urèím si dynamický rozsah IR jako rozdíl nejvyšší a nejnižší hodnoty z integrované IR
%% podle toho nastavím parametry
dbup = dbupmulti*dynrange;  %DO kdy se bude poèítat decay
dbstart = -dbstartmulti*dynrange;   %OD kdy se bude poèítat decay
dbdown = dbdownmulti*dynrange;  %posun pro crosspoint (kde se protne decay pøímka s touto hodnotou, bude nový crossponint) 

%% 2)
%zaènu tøeba na 50% délky 
t1 = ceil(length(IRs)/2);
noise = mean(env(t1:length(env)));

%% 3)
[M,predelay] = max(env);   %najdu nejvyšší hodnotu
tdecstop = find(env(predelay:length(env))<dbup+noise,1,'first')+predelay;  %poslední hodnota, kde je envelope vyšší než hladina dbup nad úrovní šumu (mùže se stát, že nastane chyba, kdyby byl v oblasti hluku nìjaký peak, ale to redukuje i integrace)
x = predelay:1:tdecstop;
y = env(predelay:tdecstop);    
p = polyfit(x.',y,1);   %koeficienty obsahují y = k*x + y ; k = p(1), q = p(2)
yy = p(1).*x + p(2);    %decay pøímka  
x1 = predelay:1:length(IRs);
yy1 = p(1).*x1 + p(2);  %decay pøímka i mimo body tdecstart (to je predelay), tdecstop

%% 5)
deltax = -dynrange/p(1);% y=k*x dy = k*dx - vypoèítám kolik vzorkù x ubìhne než se zmìní hodnota z nejvyšší na nejnížší 
                        %(kolik vzorkù, hledím-li na pøímku yy)
newpocet = 30;          %a ten rozdìlím na 30 vzrokù - pro 60dB to bude 5 intervalù na 10dB ale pøi extrémnì nízkejch 
                        %SNR to bude tøeba interval pro 0,2dB poklesu
newinterval = ceil(deltax/newpocet);    %novej interval pro integraci, nahradí pùvodní promìnnou ms

%% 6)
env = movingAverage(IRs,fs,newinterval,1);
env = log_10(env);
[M,predelay] = max(env);   %znovu najdu poèátek

newcrosspointtemp = []; %alokuju zálohu crosspoitu
for i = 1:3 %zde provedu 3 iterace, podle reference staèí maximálnì 5, pøi posledních 2 z 3 se to už fakt skoro nezmìní
%% 4)

%tohle ošetøuje to, aby vždycky byl nìjakej crosspoint, kdybych už nenašel
%hodnotu která je od pøedchozího noise flooru nižší o dbdown tak crosspoint
%posunu o polovinu blíž ke konci sekvence
newcrosspoint = find(yy1<noise-dbdown,1,'first')+ x1(1); %yy1 je ten pokles ale v té promìnné je to zapsaný od prnvího indexu ten pokles
%a plotuje se to v závislosti na x1 který to posune do doby, kde opravdu
%ten pokles je, takže musím pøièíst první prvek x1 protože ten nám
%øekne,kde ten crosspoint OPRAVDU je
if isempty(newcrosspoint) == true
    newcrosspoint = newcrosspointtemp+((length(IRs)-newcrosspointtemp)/2);
else
    newcrosspointtemp = newcrosspoint;
end

noise = mean(env(newcrosspoint:length(env)));  %urèím hladinu hluku od nového crosspointu
%% 7) new crosspoint
tdecstart = find(env>dbstart,1,'last');     %zde už se poèítá ne od zaèátku jako v sekci 3) ale od nižší hodnoty než je max, nastavené podle dbstart
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
deltax = -60/decay(1);      %kolik samplù na pokles o 60 dB
T60 = deltax/fs;

end

