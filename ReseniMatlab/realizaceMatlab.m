function [ ] = realizaceMatlab( dataset )
%Funkce, kter� provede uk�zku vypracov�n� anal�zy nam��en�ch dat
%   INPUT:
%       dataset
%           Pro zvolen� vstupn�ch dat vlo�te parametr dataset:
%               1 = men�� u�ebna SC5.48, d�lka sign�lu 700 ms
%               2 = men�� u�ebna SC5.48, d�lka sign�lu 1400 ms
%               3 = v�t�� u�ebna SC5.50, d�lka sign�lu 700 ms
%               4 = v�t�� u�ebna SC5.50, d�lka sign�lu 1400 ms            
% 
%Funkce zobraz� a� 6 grafick�ch oken
%   1 - vypo��tan� impulsn� odezvy
%   2 - doba pokelsu vypo��tan� pomoc� Lundebyho metody a Karjalainenovy
%       metody z impulsn� odezvy z�skan� z p�ela�ovan�ho harmonick�ho sign�lu
%   3 - doba pokelsu vypo��tan� pomoc� Lundebyho metody a Karjalainenovy
%       metody z impulsn� odezvy z�skan� z MLS sign�lu
%   4 - Schroederovy integr�ly impulsn�ch odezev
%   
%   5 - pokud je dataset zvolen 2
%       porovn� impulsn� odezvy z p�ela�ovan�ho harmonick�ho sign�lu
%       vypo��tan� a exportovan� z programu EASERA
%   6 - pokud je dataset zvolen 2
%       porovn� impulsn� odezvy z MLS sign�lu evpo��tan� a exportovan� z
%       programu EASERA
% 
% 
%Pokud je zvolen� dataset 2, fukce vyp�e korela�n� koeficienty, z�skan� z
%korelac� energetick�ho poklesu impulsn�ch odezev - vypo��tan�ch a
%exportovan�ch z programu EASERA.
% 
%PearsonSweep - Pearson�v korela�n� koeficient impulsn�ch odezev z
%p�ela�ovan�ho harmonick�ho sign�lu
% 
%SpearmanSweep - Spearman�v korela�n� koeficient impulsn�ch odezev z
%p�ela�ovan�ho harmonick�ho sign�lu
%
%PearsonMLS - Pearson�v korela�n� koeficient impulsn�ch odezev z MLS
%sign�lu
% 
%SpearmanMLS - Spearman�v korela�n� koeficient impulsn�ch odezev z MLS
%sign�lu

clc
close all

fs = 48000;
switch dataset
    case 1   
        n = 15;
        ms = 700;
        MLSout = audioread('mereni/merenimls15_1.wav');
        Sweepin = audioread('mereni/merenisweepin700.wav');
        Sweepout = audioread('mereni/merenisweep700_1.wav'); 
    case 2
        n = 16;
        ms = 1400;
        MLSout = audioread('mereni/merenimls16_1.wav');
        Sweepin = audioread('mereni/merenisweepin1400.wav');
        Sweepout = audioread('mereni/merenisweep1400_1.wav');
    case 3
        n = 15;
        ms = 700;
        MLSout = audioread('mereni/merenimls15_2.wav');
        Sweepin = audioread('mereni/merenisweepin700.wav');
        Sweepout = audioread('mereni/merenisweep700_2.wav');

    case 4
        n = 16;
        ms = 1400;
        MLSout = audioread('mereni/merenimls16_2.wav');
        Sweepin = audioread('mereni/merenisweepin1400.wav');
        Sweepout = audioread('mereni/merenisweep1400_2.wav');
    otherwise
        return
end

%% ur�en� impulsn� odezvy

IRsweep = computeIRSweep(Sweepin,Sweepout);
IRMLS = computeIRMLS(MLSout,n);

% filtr kv�li vyhlazen� ru�iv�ho nap�t�
IRsweep = filterIR(IRsweep,fs);

figure(1)
addToSubplot(IRsweep,fs,2,1,1,'Impulsn� odezva z�skan� z p�ela�ovan�ho harmonick�ho sign�lu',0,0.5);
addToSubplot(IRMLS,fs,2,1,2,'Impulsn� odezva z�skan� z MLS',0,0.5);

%% ur�en� doby odzvuku

for i = 1:2
    if i == 1
        IR = IRsweep;
    else
        IR = IRMLS;
    end
    
t = 1:length(IR);
t = t.';

%% Lundebyho metoda

[env, predelay, decay, noise, crosspoint, T60] = lundebysMethodFun(IR,fs);
%env je na druhou umocn�n� normalizovan� a po ��stech zintegrovan� IR
%predelay - �as od kdy se po��t� decay
%decay - nese parametry p��mky poklesu
%noise - hladina prahu hluku
%crosspoint - bod, od kter�ho se do konce IR zpr�m�ruje hladina hluku z IR, nebo env
if i==1
    crosspointsweep = crosspoint; %pozd�ji se budou crosspointy hodit k ur�ov�n� Schroedrova integr�lu
else crosspointmls = crosspoint;
end

figure(i+1)
addToSubplot(env,fs,2,1,1,'Pokles z�skan� Lundebyho metodou',0,0.5, noise-5,1)
ylabel('\it e(t) \rm[dB]','Interpreter','LaTeX')
hold on
plot(t./fs,decay(1).*t + decay(2), 'r--')        %vykresl�m decay
plot(t./fs,noise*ones([1,length(IR)]),'g--')     %vykresl�m hladinu hluku
hold off

str = sprintf('T_{60} = %0.3f s',T60);          %hodnota T60
text(0.2,-20,str)

%% Karjalainenova metoda

[z,coeffs,T60k] = karjalainenEnvFit(IR,0.4,fs);
addToSubplot(log_10(IR.^2),fs,2,1,2,'Pokles z�skan� Karjalainenovou metodou',0,0.5,min(z(:,1))-15, 1);
ylabel('\it e(t) \rm[dB]','Interpreter','LaTeX')
hold on
ydata = z(:,1);     %ze z z�sk�m x a y data exponenci�ln�ho poklesu
xdata = z(:,2);
plot(xdata./fs,ydata,'r--')

str = sprintf('T_{60} = %0.3f s',T60k);
text(0.2,-20,str)
hold off

end

%% Schroeder�v integr�l

figure(4)
[schroedersweep,decaysweep,T60sw] = schroederIR(IRsweep(1:crosspointsweep).^2,1,fs);
ylabel('\it Schr(t) \rm[dB]','Interpreter','LaTeX')
addToSubplot(schroedersweep,fs,2,1,1,'Schroedr�v integr�l ze sweepu',0,0.5,-80,10);
hold on
regsweep = t.*decaysweep(1) + decaysweep(2);
plot(t./fs,regsweep, 'r--')
str = sprintf('T_{60} = %0.3f s',T60sw);
text(0.1,0,str)
hold off

[schroedermls,decaymls,T60mls] = schroederIR(IRMLS(1:crosspointmls).^2,1,fs);
addToSubplot(schroedermls,fs,2,1,2,'Schroedr�v integr�l z MLS',0,0.5,-80,10);
ylabel('\it Schr(t) \rm[dB]','Interpreter','LaTeX')
hold on
regmls = t.*decaymls(1) + decaymls(2);
plot(t./fs,regmls, 'r--')
str = sprintf('T_{60} = %0.3f s',T60mls);
text(0.1,0,str)
hold off

clc
if dataset ~=2
    return;
end

%% porovn�n� EASERY a m�ho k�du
IRsweep = IRsweep/(max(IRsweep));       %normalizace, EASERA to d�l� automaticky
IRMLS = IRMLS/(max(IRMLS));
IRsweepE = audioread('mereni/IRsweep1,4s.wav');
IRMLSE = audioread('mereni/IRMLS16.wav');

figure(5)
title = sprintf('Impulsn� odezva z�skan� z p�ela�ovan�ho harmonick�ho sign�lu %d ms',ms);
addToSubplot(IRsweep,fs,2,1,1,title, 0,0.5);
addToSubplot(IRsweepE,fs,2,1,2,'Impulsn� odezva z�skan� z p�ela�ovan�ho harmonick�ho sign�lu programam EASERA 1200ms', 0,0.5);

figure(6)
title = sprintf('Impulsn� odezva z�skan� z z�skan� z MLS sign�lu ��du %d',n);
addToSubplot(IRMLS,fs,2,1,1,title, 0,0.5);
addToSubplot(IRMLSE,fs,2,1,2,'Impulsn� odezva z�skan� z MLS sign�lu ��du 16 programam EASERA', 0,0.5);

%% pro porovn�n� v�sledk� poslou�� Pearson�v a Spearman�v korela�n� koeficient
%Pearson�v korela�n� koeficient slou�� k porovn�n� dat s line�rn�
%z�vislost�. Liner�rn� pokles nalezneme v k�ivce ETD - energy time decay
%Line�rn� pokels je jen v po��te�n� ��sti impulsn� odezvy, pot� u� je
%hladina hluku pozad�

ETDsw = log_10(IRsweep(300:15000).^2);
ETDswE = log_10(IRsweepE(300:15000).^2);

%pokud jsou hodnoty impulsn� odezvy bl�zk� nule, logaritmus vr�t� -Inf.
%Tyto hodnoty je t�eba eliminovat, jinak nebude korelace �sp�n�.
indicies = find(ETDswE==-Inf);
ETDswE(indicies) = -100;        %nastav�m na n�zkou hodnotu

pearsonSweep = corrcoef(ETDsw, ETDswE);
PearsonSweep = pearsonSweep(2,1)
SpearmanSweep = corr(ETDsw, ETDswE,'Type','Spearman')

%-----------
ETDmls = log_10(IRMLS(300:10000).^2);
ETDmlsE = log_10(IRMLSE(300:10000).^2);

indicies = find(ETDmlsE==-Inf);
ETDmlsE(indicies) = -100;        %nastav�m na n�zkou hodnotu

pearsonMLS = corrcoef(ETDmls, ETDmlsE);
PearsonMLS = pearsonMLS(2,1)
SpearmanMLS = corr(ETDmls, ETDmlsE,'Type','Spearman')


end

