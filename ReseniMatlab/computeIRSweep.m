function [ IR ] = ComputeIRSweep( in, out )
%Vypoèítá impulsní odezvu z vstupního a výstupního signálu sweep
%   INPUT:
%   in = vstpuní sweep sigál
%   out = nahraný výstupní signál
%
%   OUTPUT
%   IR = impulsní odezva 

nfft = length(in);
n = 2^nextpow2(nfft); %pro 2^x delku fft

In = fft(in,n);
Out = fft(out,n);

% http://www.ni.com/white-paper/4278/en/ 5. Frequency response function
Sbb = (Out.*conj(Out))/(n^2);
Sab = (In.*conj(Out))/(n^2);

H2 = Sbb./Sab;
IR = ifft(H2,n);

end

