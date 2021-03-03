function [expanededMeasurements] = expandRange(measurements)
    correction = measurements(1);
    previous = measurements(1);
    offset = 0;
    for i = 1 : length(measurements)
        delta = measurements(i) - previous;
        previous = measurements(i);
        if delta < -300
            offset = offset + 360;
        elseif delta > 300
            offset = offset - 360;
        end
        expanededMeasurements(i) = measurements(i) + offset - correction ;
    end
end