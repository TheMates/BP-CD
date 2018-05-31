function [ ] = addToSubplot( data,fs,rows,cols,position,newtitle,Xmin,Xmax,Ymin,Ymax )
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here

t = 1:length(data);
t = t.';
subplot(rows, cols, position)
plot(t./fs,data)
title(newtitle);

xlabel('\textit{t} [s]','Interpreter','LaTeX')
ylabel('\textit{h(t)}','Interpreter','LaTeX')
ax = gca;
ax.TickLabelInterpreter = 'latex';
tit = get(gca,'title');
set(gca,'fontsize',12)
if(nargin>6)
   xlim([Xmin Xmax]);
end
if(nargin>8)
    ylim([Ymin Ymax]);
end

end

