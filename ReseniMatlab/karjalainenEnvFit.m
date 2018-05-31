function [ z, coeffs, T60] = KarjalainenEnvFit( IR, expf ,fs)
%Funkce používající model Mattiho Karjaleinena pro proložení IR modelem,
%který zahrnuje klesající exponenciálu a stabilní hladinu hluku - rovnice
%a(t) = sqrt(A^2*exp(-2*tau*t) + An^2) , kde t je èas, A, tau, An jsou parametry, které se vypoèítají
%pomocí LS fitting
%   Detailed explanation goes here
%   INPUT:
%   IR = impulsní odezva
%   expf = koeficient s (Estimation of Modal Parameters from Noisy Response Measurements - eqn. 16)
%   fs = vzorkovací kmitoèet
%
%   OUTPUT:
%   z = modelová proložená funkce, kterou už rovnou mùžu kreslit z(1) = y, z(2) = x
%   coeffs = parametry A, tau, An (Estimation of Modal Parameters from Noisy Response Measurements - eqn. 13)
%           coeffs(1) = A, coeffs(2) = -2tau, coeffs(3) = An
%   T60 = doba poklesu vypoèítaná z poèáteèní lineární èásti poklesové køivky

if nargin<2 expf=0.4; end   %default

IR = log_10(IR.^2);    % logaritmicky a ještì normalizovanì
[val,indx]=max(IR);  % najdu maximum

sig = IR(indx:length(IR));      %budu pracovat jen s tím, co chci opravdu prokládat, když jsem pracoval s oknem w tak mi to moc nefungovalo
                                %jako vstupní data tedy dám impulsní odezvu od nejvyšší hodnoty do konce
[coeffs, norm,z]=decay2_fit(sig,expf,[]); %bez ètvrtýho parametru - nechci kreslit figure
z(:,2) = indx:length(IR);

[maxv,maidx] = max(z(:,1)); %nejvyšší hodnota køivky
[minv,miidx] = min(z(:,1)); %nejnižší hodnota køivky

dynrange = maxv-minv;   %dynrange té poklesové køivky
halfrange = find(z(:,1)<(-dynrange/2)+z(maidx,1),1,'first');  %index hodnoty, kde to klesne na polovinu, do té doby by se ten pokles mohl dost blížit pøímce
deltax = halfrange*(-60/(-dynrange/2));  %vynásobím tak, abych z Thalfrange mìl T60
T60 = deltax/fs;

end


%% funkce od Matti Karjalainena z http://legacy.spa.aalto.fi/software/decay/ (new version)


function [v,norm,z]=decay2_fit(data,expf,w,figno)   %upraveno, pùvodnì funkce nevracela z

% fit decay_model to dB-scaled energy-time decay curve
% data(:,1) = decay data (dB scaled)
% data(:,2) = optional time sample points
% expf = optional weighting factor
% w = weighting vector, default equal weights
% figno = optional figure number, 0=no figure
% v = model coefficients
% norm = error norm from curvefit

sizd=size(data);                      % data dimensionality
sn=sizd(1); sn2=ceil(sn/2);           % size, size/2
sn10=ceil(sn/10);                     % size/10
if (nargin<4) figno = 0; end          % figure to draw to or none=0
if (nargin<3)|(isempty(w))
	w=ones(sn,1); end                 % weight vector
if (nargin<2) expf=0.4; end           % expf for model function
if isempty(expf) expf=0.4; end        % expf for model function
%
y=data(:,1);                          % dB scaled magnitudes
if (sizd(2)<2) x=(1:sizd(1))';        % time value vector default
else x=data(:,2);                     % optionally given time points
end
ydata=(10.^(y./20)).^expf;            % magnitude data scaled
nz=find(w); 
bi=nz(1);                 % start index (bi) of nonzero weight
y1=mean(ydata(bi:sn10+bi));           % average of first 10% (after bi)
yn=mean(ydata(sn-sn10+1:sn));         % average of last 10%
tmat = [ones(sn2,1) x(bi:sn2+bi-1)];

%%%%mùj nápad
%nìkdy se stane, že pøi pøevodu do logaritmickýho mìøítka narazím na
%hodnotu pøesnì 0 -> dá mi to -Inf, tak to z toho musim dat pryè a pak dìlat
%ten polyfit nebo ten operator \ 
%tìch dat je vìtšinou minimálnì, takže délka arraye zùstane v podstatì
%stejná
ytemp = y;                            
indicies = find(ytemp==-Inf);
ytemp(indicies) = [];
%%%%

p=tmat\ytemp(bi:sn2+bi-1);                % regression line 50% after bi
% p=tmat\y(bi:sn2+bi-1);                % regression line 50% after bi

tau0=p(2)/8.7;                        % decay parameter initial value
v0=[y1 tau0 yn];                      % vector of variables initial value
ydata=w.*ydata;                       % ydata weighted
%
[v,norm]=lsqcurvefit('decay_model',v0,x,ydata,[],[],[],expf,w); % optimize
%                                     % v = model coefficients
if (figno>0) plot(x,y); hold on; end  % plot target data if figno~=0
z=20*log10(decay_model(v,x,1));
if (figno>0) plot(x,z,'r'); hold off; end  % plot fitted data if figno~=0
end
% Call this way: decay2_fit(vp(:,40),[],[],1)

