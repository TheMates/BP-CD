function [] = WriteTags(in)

fileID = fopen('C:\Users\Matou�\Documents\�kola\6. semestr\bakal��ka\c# proj\IRWForm\IRWForm\tags\P1(9).txt','w');
fwrite(fileID,in,'int32');
fclose(fileID);
end