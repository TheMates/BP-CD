function [ y ] = log_10( x )
%Funkce, která pøevede data na 10*log(x) (10 protože chcemem energii) a
%zároveò maximální hodnota bude v 0
%   x = data
y = 10*log10(x/max(abs(x)));

end

