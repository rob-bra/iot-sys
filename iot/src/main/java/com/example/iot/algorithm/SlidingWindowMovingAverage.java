package com.example.iot.algorithm;

public class SlidingWindowMovingAverage {

    private final double[] buffer;
    private final int windowSize;
    private int index;
    private int count;
    private double sum;

    public SlidingWindowMovingAverage(int windowSize) {
        if (windowSize <= 0) {
            throw new IllegalArgumentException("windowSize must be > 0");
        }
        this.windowSize = windowSize;
        this.buffer = new double[windowSize];
        this.index = 0;
        this.count = 0;
        this.sum = 0.0;
    }

    public double add(double value) {
        if (count < windowSize) {
            buffer[index] = value;
            sum += value;
            count++;
        } else {
            sum -= buffer[index];
            buffer[index] = value;
            sum += value;
        }

        index = (index + 1) % windowSize;
        return getAverage();
    }

    public double getAverage() {
        if (count == 0) {
            return 0.0;
        }
        return sum / count;
    }
}