function [settedData, measuredData] = readFile(fileName)

figure
hold on;
grid on;
fid = fopen(fileName);
for i = 1 : 2
    tline = fgetl(fid);
end
tmp = sscanf(tline,'%f %f');
settedData = tmp(1);
measuredData = tmp(2);
tline = fgetl(fid);
while ischar(tline)
    % disp(tline)
    tmp = sscanf(tline,'%f %f');
    settedData = [settedData, tmp(1)];
    measuredData = [measuredData, tmp(2)];
    tline = fgetl(fid);
end
measuredData = expandRange(measuredData);
fclose(fid);
clear tmp tline fid i 
t = [1 : length(measuredData)] / 100;
plot(t, measuredData, t, settedData)
legend('measure [deg]','set value [%]');
title(fileName);
clear ans t
settedData = settedData;
measuredData = measuredData'
settedData = settedData'
% clear settedData measuredData

end