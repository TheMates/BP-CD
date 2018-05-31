function [ ] = realizaceMatlab( dataset )
%Funkce, která provede ukázku vypracování analýzy namìøených dat
%   INPUT:
%       dataset
%           Pro zvolení vstupních dat vložte parametr dataset:
%               1 = menší uèebna SC5.48, délka signálu 700 ms
%               2 = menší uèebna SC5.48, délka signálu 1400 ms
%               3 = vìtší uèebna SC5.50, délka signálu 700 ms
%               4 = vìtší uèebna SC5.50, délka signálu 1400 ms            
% 
%Funkce zobrazí až 6 grafických oken
%   1 - vypoèítané impulsní odezvy
%   2 - doba pokelsu vypoèítaná pomocí Lundebyho metody a Karjalainenovy
%       metody z impulsní odezvy získané z pøelaïovaného harmonického signálu
%   3 - doba pokelsu vypoèítaná pomocí Lundebyho metody a Karjalainenovy
%       metody z impulsní odezvy získané z MLS signálu
%   4 - Schroederovy integrály impulsních odezev
%   
%   5 - pokud je dataset zvolen 2
%       porovná impulsní odezvy z pøelaïovaného harmonického signálu
%       vypoèítané a exportované z programu EASERA
%   6 - pokud je dataset zvolen 2
%       porovná impulsní odezvy z MLS signálu evpoèítané a exportované z
%       programu EASERA
% 
% 
%Pokud je zvolený dataset 2, fukce vypíše korelaèní koeficienty, získané z
%korelací energetického poklesu impulsních odezev - vypoèítaných a
%exportovaných z programu EASERA.
% 
%PearsonSweep - Pearsonùv korelaèní koeficient impulsních odezev z
%pøelaïovaného harmonického signálu
% 
%SpearmanSweep - Spearmanùv korelaèní koeficient impulsních odezev z
%pøelaïovaného harmonického signálu
%
%PearsonMLS - Pearsonùv korelaèní koeficient impulsních odezev z MLS
%signálu
% 
%SpearmanMLS - Spearmanùv korelaèní koeficient impulsních odezev z MLS
%signálu

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

%% urèení impulsní odezvy

IRsweep = computeIRSweep(Sweepin,Sweepout);
IRMLS = computeIRMLS(MLSout,n);

% filtr kvùli vyhlazení rušivého napìtí
IRsweep = filterIR(IRsweep,fs);

figure(1)
addToSubplot(IRsweep,fs,2,1,1,'Impulsní odezva získaná z pøelaïovaného harmonického signálu',0,0.5);
addToSubplot(IRMLS,fs,2,1,2,'Impulsní odezva získaná z MLS',0,0.5);

%% urèení doby odzvuku

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
%env je na druhou umocnìná normalizovaná a po èástech zintegrovaná IR
%predelay - èas od kdy se poèítá decay
%decay - nese parametry pøímky poklesu
%noise - hladina prahu hluku
%crosspoint - bod, od kterého se do konce IR zprùmìruje hladina hluku z IR, nebo env
if i==1
    crosspointsweep = crosspoint; %pozdìji se budou crosspointy hodit k urèování Schroedrova integrálu
else crosspointmls = crosspoint;
end

figure(i+1)
addToSubplot(env,fs,2,1,1,'Pokles získaný Lundebyho metodou',0,0.5, noise-5,1)
ylabel('\it e(t) \rm[dB]','Interpreter','LaTeX')
hold on
plot(t./fs,decay(1).*t + decay(2), 'r--')        %vykreslím decay
plot(t./fs,noise*ones([1,length(IR)]),'g--')     %vykreslím hladinu hluku
hold off

str = sprintf('T_{60} = %0.3f s',T60);          %hodnota T60
text(0.2,-20,str)

%% Karjalainenova metoda

[z,coeffs,T60k] = karjalainenEnvFit(IR,0.4,fs);
addToSubplot(log_10(IR.^2),fs,2,1,2,'Pokles získaný Karjalainenovou metodou',0,0.5,min(z(:,1))-15, 1);
ylabel('\it e(t) \rm[dB]','Interpreter','LaTeX')
hold on
ydata = z(:,1);     %ze z získám x a y data exponenciálního poklesu
xdata = z(:,2);
plot(xdata./fs,ydata,'r--')

str = sprintf('T_{60} = %0.3f s',T60k);
text(0.2,-20,str)
hold off

end

%% Schroederùv integrál

figure(4)
[schroedersweep,decaysweep,T60sw] = schroederIR(IRsweep(1:crosspointsweep).^2,1,fs);
ylabel('\it Schr(t) \rm[dB]','Interpreter','LaTeX')
addToSubplot(schroedersweep,fs,2,1,1,'Schroedrùv integrál ze sweepu',0,0.5,-80,10);
hold on
regsweep = t.*decaysweep(1) + decaysweep(2);
plot(t./fs,regsweep, 'r--')
str = sprintf('T_{60} = %0.3f s',T60sw);
text(0.1,0,str)
hold off

[schroedermls,decaymls,T60mls] = schroederIR(IRMLS(1:crosspointmls).^2,1,fs);
addToSubplot(schroedermls,fs,2,1,2,'Schroedrùv integrál z MLS',0,0.5,-80,10);
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

%% porovnání EASERY a mého kódu
IRsweep = IRsweep/(max(IRsweep));       %normalizace, EASERA to dìlá automaticky
IRMLS = IRMLS/(max(IRMLS));
IRsweepE = audioread('mereni/IRsweep1,4s.wav');
IRMLSE = audioread('mereni/IRMLS16.wav');

figure(5)
title = sprintf('Impulsní odezva získaná z pøelaïovaného harmonického signálu %d ms',ms);
addToSubplot(IRsweep,fs,2,1,1,title, 0,0.5);
addToSubplot(IRsweepE,fs,2,1,2,'Impulsní odezva získaná z pøelaïovaného harmonického signálu programam EASERA 1200ms', 0,0.5);

figure(6)
title = sprintf('Impulsní odezva získaná z získaná z MLS signálu øádu %d',n);
addToSubplot(IRMLS,fs,2,1,1,title, 0,0.5);
addToSubplot(IRMLSE,fs,2,1,2,'Impulsní odezva získaná z MLS signálu øádu 16 programam EASERA', 0,0.5);

%% pro porovnání výsledkù poslouží Pearsonùv a Spearmanùv korelaèní koeficient
%Pearsonùv korelaèní koeficient slouží k porovnání dat s lineární
%závislostí. Linerární pokles nalezneme v køivce ETD - energy time decay
%Lineární pokels je jen v poèáteèní èásti impulsní odezvy, poté už je
%hladina hluku pozadí

ETDsw = log_10(IRsweep(300:15000).^2);
ETDswE = log_10(IRsweepE(300:15000).^2);

%pokud jsou hodnoty impulsní odezvy blízké nule, logaritmus vrátí -Inf.
%Tyto hodnoty je tøeba eliminovat, jinak nebude korelace úspìšná.
indicies = find(ETDswE==-Inf);
ETDswE(indicies) = -100;        %nastavím na nízkou hodnotu

pearsonSweep = corrcoef(ETDsw, ETDswE);
PearsonSweep = pearsonSweep(2,1)
SpearmanSweep = corr(ETDsw, ETDswE,'Type','Spearman')

%-----------
ETDmls = log_10(IRMLS(300:10000).^2);
ETDmlsE = log_10(IRMLSE(300:10000).^2);

indicies = find(ETDmlsE==-Inf);
ETDmlsE(indicies) = -100;        %nastavím na nízkou hodnotu

pearsonMLS = corrcoef(ETDmls, ETDmlsE);
PearsonMLS = pearsonMLS(2,1)
SpearmanMLS = corr(ETDmls, ETDmlsE,'Type','Spearman')


end

