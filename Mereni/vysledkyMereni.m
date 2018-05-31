function [] = vysledkyMereni( dataset )
%Funkce, která provede ukázku vypracování analýzy namìøených dat
%   INPUT:
%       dataset
%           Pro zvolení vstupních dat vložte parametr dataset:
%               1 = uèebna SC5.50, harmonický pøelaïovaný signál 700 ms (fs = 48kHz)
%               2 = uèebna SC5.50, harmonický pøelaïovaný signál 1400 ms (fs = 48kHz)
%               3 = uèebna SC5.50, MLS signál o øádu 15 (fs = 48kHz)
%               4 = uèebna SC5.50, MLS signál o øádu 15 (fs = 48kHz)          
%               5 = uèebna SC5.50, MLS signál o øádu 17 (fs = 96kHz)          
% 
%Funkce zobrazí až 3 grafických oken
%   1 - vypoèítané impulsní odezvy
%   2 - poklesy a doby dozvuku vypoèítané pomocí Lundebyho metody 
%   3 - Schroederovy integrály impulsních odezev
% 
% 
%Fukce vypíše korelaèní koeficienty, získané z korelací Schroederových itegrálù 
%vypoèítané impulsní odezvy a exportované z programu EASERA.
% 
%PearsonSchroeder - Pearsonùv korelaèní koeficient Schroederových integrálù
% 
%SpearmanSchroeder - Spearmanùv korelaèní koeficient Schroederových integrálù 
%

fs = 48000;
switch dataset
    case 1
        data = load('ir683_4r.mat');
        IR = data.ir683_4r;
        data = load('ir683_4rE.mat');
        IRe = data.ir683_4rE;
        xMax = 0.6;
    case 2
        data = load('ir1400_4r.mat');
        IR = data.ir1400_4r;
        data = load('ir1400_4rE.mat');
        IRe = data.ir1400_4rE;
        xMax = 0.6;
    case 3
        data = load('irmls15_4r.mat');
        IR = data.irmls15_4r;
        data = load('irmls15_4rE.mat');
        IRe = data.irmls15_4rE;
        xMax = 0.3;
    case 4 
        data = load('irmls16_4r.mat');
        IR = data.irmls16_4r;
        data = load('irmls16_4rE.mat');
        IRe = data.irmls16_4rE;
        xMax = 0.4;
    case 5
        fs = 96000;
        data = load('irmls17_4r96.mat');
        IR = data.irmls17_4r96;
        data = load('irmls17_4r96E.mat');
        IRe = data.irmls17_4r96E;
        xMax = 0.3;
    otherwise
        return;
end

t = 1:length(IR);
t = t.';

figure(1)
addToSubplot(IR,fs,1,1,1,[],0,xMax)
hold on
addToSubplot(IRe,fs,1,1,1,'Impulsní odezvy',0,xMax)
hold off
legend('Impulsní odezva','Impulsní odezva - EASERA');

figure(2)
[env, predelay, decay, noise, crosspoint, T60] = lundebysMethodFun(IR,fs);

addToSubplot(env,fs,1,1,1,'Pokles získaný Lundebyho metodou',0,xMax, noise-5,1)
hold on
plot(t./fs,decay(1).*t + decay(2), 'r--')        %vykreslím decay
plot(t./fs,noise*ones([1,length(IR)]),'r--')     %vykreslím hladinu hluku


str = sprintf('T_{60} = %0.3f s',T60);          %hodnota T60
text(0.2,-10,str)


t = 1:length(IRe);
t = t.';

[enve, predelay, decay, noise, crosspointe, T60] = lundebysMethodFun(IRe,fs);

addToSubplot(enve,fs,1,1,1,'Pokles získaný Lundebyho metodou',0,xMax, noise-5,1)

plot(t./fs,decay(1).*t + decay(2), 'g--')        %vykreslím decay
plot(t./fs,noise*ones([1,length(IRe)]),'g--')     %vykreslím hladinu hluku
hold off
ylabel('\it ETC(t) \rm[dB]','Interpreter','LaTeX')
str = sprintf('T_{60 EASERA} = %0.3f s',T60);          %hodnota T60
text(0.25,-20,str)
legend('ETC(t)','regrese T_{60}','úroveò hluku','ETC(t) - EASERA','regrese T_{60} - EASERA','úroveò hluku - EASERA');


figure(3)
sch = schroederIR(IR(1:crosspoint).^2,1,fs);
sche = schroederIR(IRe(1:crosspoint).^2, 1, fs);

[T10moje,T20moje,T30moje] = computeTdecay(sch,fs)
[T10easera,T20easera,T30easera] = computeTdecay(sche,fs)
  
addToSubplot(sch,fs,1,1,1,[],0,0.7, noise-15,1)
hold on
addToSubplot(sche,fs,1,1,1,'Schroederùv integrál',0,xMax, noise-20,1)
hold off
ylabel('\it sch(t) \rm[dB]','Interpreter','LaTeX')
legend('Schroederùv integrál','Schroederùv integrál EASERA');

pearson = corrcoef(sch, sche);
PearsonSchroeder = pearson(2,1)
SpearmanSchroeder = corr(sch, sche,'Type','Spearman')


end



