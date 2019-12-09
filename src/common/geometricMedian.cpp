#include "geometricMedian.h"

#include <iostream>

double distSum(const cv::Point2f& p, const std::vector<cv::Point2f>& points)
{
    float sum = 0;
    for (std::size_t i = 0; i < points.size(); i++)
    {
        auto distx = points[i].x - p.x;
        auto disty = points[i].y - p.y;
        sum += std::sqrt((distx * distx) + (disty * disty));
    }

    // Return the sum of Euclidean Distances
    return sum;
}

cv::Point2f geometricMedian(const std::vector<cv::Point2f>& points)
{

    // Current x coordinate and y coordinate
    cv::Point2f current_point;
    for (std::size_t i = 0; i < points.size(); i++)
    {
        current_point.x += points[i].x;
        current_point.y += points[i].y;
    }

    current_point.x /= points.size();
    current_point.y /= points.size();

    double minimum_distance = distSum(current_point, points);

    // check if any point has a smaller distance to all other points
    for (std::size_t i = 0; i < points.size(); i++)
    {
        cv::Point2f newpoint;
        newpoint.x = points[i].x;
        newpoint.y = points[i].y;
        double newd = distSum(newpoint, points);
        if (newd < minimum_distance)
        {
            minimum_distance = newd;
            current_point.x = newpoint.x;
            current_point.y = newpoint.y;
        }
    }

    // Assume test_distance to be 1000
    float test_distance = 1000;
    bool flag = false;

    cv::Point2f test_point[] = {{-1.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, {0.0, -1.0}};
    // Test loop for approximation starts here
    while (test_distance > 0.001)
    {
        flag = false;

        // Loop for iterating over all 4 neighbours
        for (int i = 0; i < 4; i++)
        {
            // Finding Neighbours done
            cv::Point2f newpoint;
            newpoint.x = current_point.x + test_distance * test_point[i].x;
            newpoint.y = current_point.y + test_distance * test_point[i].y;

            // New sum of Euclidean distances
            // from the neighbor to the given
            // data points
            double newd = distSum(newpoint, points);

            if (newd < minimum_distance)
            {
                // Approximating and changing
                // current_point
                minimum_distance = newd;
                current_point.x = newpoint.x;
                current_point.y = newpoint.y;
                flag = true;
                break;
            }
        }

        // This means none of the 4 neighbours
        // has the new minimum distance, hence
        // we divide by 2 and reiterate while
        // loop for better approximation
        if (!flag)
            test_distance /= 2;
    }

    std::cout << "Geometric Median = (" << current_point.x << ", " << current_point.y << ")";
    std::cout << " with minimum distance = " << minimum_distance << std::endl;
    return current_point;
}
