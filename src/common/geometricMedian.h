#ifndef GEOMETRIC_MEDIAN_H
#define GEOMETRIC_MEDIAN_H

#include <opencv2/core.hpp>

// Function to return the sum of Euclidean
// Distances
double distSum(const cv::Point2f& p, const std::vector<cv::Point2f>& arr);

// Function to calculate the required
// geometric median
cv::Point2f geometricMedian(const std::vector<cv::Point2f>& points);

#endif // GEOMETRIC_MEDIAN_H
