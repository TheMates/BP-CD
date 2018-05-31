function [] = WriteTags(in)

fileID = fopen('C:\Users\Matouš\Documents\Škola\6. semestr\bakaláøka\c# proj\IRWForm\IRWForm\tags\P1(9).txt','w');
fwrite(fileID,in,'int32');
fclose(fileID);
end