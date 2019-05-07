#include <iostream>
#include <numeric>
#include <tuple>
#include <vector>

#define kRand (static_cast<float>(rand()) / static_cast<float>(RAND_MAX))
#define kZrand (kRand - 0.5)
#define kVelocityNoise 1  // TODO: tune noise values
#define kYawNoise 0.005
#define kSamples 1000

using namespace std;

struct Point {
    double x;
    double y;
};

struct Pose {
    Point pos;
    double yaw;
};

struct Segment {
    Point a;
    Point b;
};

// TODO: Add the other four sections
const Point tar_right_rocket_near = {213.57, 17.93};
const Point tar_right_rocket_center = {228.28, 27.44};
const Point tar_right_rocket_far = {242.99, 17.93};
const Point tar_right_ship_near = {259.8, 133.13};
const Point tar_right_ship_center = {281.55, 133.13};
const Point tar_right_ship_far = {-303.3, 133.13};
const Point tar_right_ship_front = {220.25, 151.13};
const Point tar_right_playerstation = {0, 25.72};
const vector<Point> all_targets = {
    tar_right_rocket_near, tar_right_rocket_center, tar_right_rocket_far,
    tar_right_ship_near,   tar_right_ship_center,   tar_right_ship_far,
    tar_right_ship_front,  tar_right_playerstation};
const Segment seg_right_ship_side = {{220.25, 133.13}, {323.81, 133.13}};
const Segment seg_right_ship_near = {{220.25, 133.13}, {220.25, 188.13}};
const Segment seg_right_rocket_center = {{218.56, 27.44}, {238.00, 27.44}};
const Segment seg_right_rocket_near = {{218.56, 27.44}, {207.75, 7.82}};
const Segment seg_right_rocket_backing = {{207.75, 7.82}, {248.7, 7.82}};
const Segment seg_right_rocket_far = {{248.7, 7.82}, {238.00, 27.44}};
const vector<Segment> segment_map = {
    seg_right_ship_side,   seg_right_ship_near,      seg_right_rocket_center,
    seg_right_rocket_near, seg_right_rocket_backing, seg_right_rocket_far};

vector<Point> expected_targets(Pose pose) {}

double RatePrediction(vector<Point> field, Pose prediction) {
    // TODO: Accuracy based on comparing the sorted shortest lengths
}

Pose NewPoseWithNoise(Pose previous) {  // TODO: Implement dx to more
                                        // accurately predict new poses.
    Pose new_pose;
    new_pose.pos.x = previous.pos.x + (kZrand * kVelocityNoise);
    new_pose.pos.y = previous.pos.y + (kZrand * kVelocityNoise);
    new_pose.yaw = previous.yaw + (kZrand * kYawNoise);
}

Pose *MCLStep(vector<Pose> previous_predictions,
              vector<Point> measured_targets) {
    // Compare predictions to measured
    double weights[kSamples];
    for (int i = 0; i < kSamples; i++) {
        weights[i] = RatePrediction(measured_targets, previous_predictions[i]);
    }
    // Generate Random predictions around probabilities
    Pose new_predictions[kSamples];
    double weightsSum = accumulate(begin(weights), end(weights), 0.0);
    for (int i = 0; i < kSamples; i++) {
        float weight = weightsSum * kRand;
        for (int j = 0; j < kSamples; j++) {
            weight -= weights[j];
            if (weight <= 0) {
                new_predictions[i] = NewPoseWithNoise(previous_predictions[j]);
                break;
            }
        }
    }
    return new_predictions;
}

int main() {
    // TODO: Implement test cases
}
