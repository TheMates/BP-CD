function [ H ] = hadamard( n, flag)
%hadamard1(n, flag) -  funkce která vrátí matici Sylvesterova typu o velikosti 2^n
%   n = øád matice (2^n)
%   flag = 0 pro matici obsahující 0 a 1, 1 promatici obsahující -1 a 1

switch flag
    case 0
        H = false;
        if n ==0
            return
        else
            for i = 0:n-1
            H = [H H; H ~H];
            i = i+1;
            end
        end
    case 1
        H = int8(1);
        if n ==0
            return
        else
            for i = 0:n-1
            H = [H H; H -H];
            i = i+1;
            end
        end
end

