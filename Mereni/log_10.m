function [ y ] = log_10( x )
%Funkce, kter� p�evede data na 10*log(x) (10 proto�e chcemem energii) a
%z�rove� maxim�ln� hodnota bude v 0
%   x = data
y = 10*log10(x/max(abs(x)));

end

