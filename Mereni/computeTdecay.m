function [ T10, T20, T30 ] = computeTdecay( sch, fs )
dbstart = -5;
db10 = -15;
db20 = -25;
db30 = -35;

tstart =find(sch<dbstart,1,'first');

t10 = find(sch<db10,1,'first');
t20 = find(sch<db20,1,'first');
t30 = find(sch<db30,1,'first');


x10 = tstart:1:t10;
x20 = tstart:1:t20;
x30 = tstart:1:t30;
y10 = sch(tstart:t10);
y20 = sch(tstart:t20);
y30 = sch(tstart:t30);
p10 = polyfit(x10.',y10,1);
p20 = polyfit(x20.',y20,1);
p30 = polyfit(x30.',y30,1);

T10 = (-60/p10(1))/fs;
T20 = (-60/p20(1))/fs;
T30 = (-60/p30(1))/fs;
end

