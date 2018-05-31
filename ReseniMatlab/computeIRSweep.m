function [ IR ] = ComputeIRSweep( in, out )
%Vypo��t� impulsn� odezvu z vstupn�ho a v�stupn�ho sign�lu sweep
%   INPUT:
%   in = vstpun� sweep sig�l
%   out = nahran� v�stupn� sign�l
%
%   OUTPUT
%   IR = impulsn� odezva 

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

