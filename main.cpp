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

// TODO: Add the other four sections
const Point rocket_right_near = {213.57, 17.93};
const Point rocket_right_center = {228.28, 27.44};
const Point rocket_right_far = {242.99, 17.93};
const Point ship_right_near = {259.8, 133.13};
const Point ship_right_center = {281.55, 133.13};
const Point ship_right_far = {-303.3, 133.13};
const Point ship_right_front = {220.25, 151.13};
const Point player_station_right = {0, 25.72};
const vector<Point> all_targets = {rocket_right_near, rocket_right_center,
                                   rocket_right_far,  ship_right_near,
                                   ship_right_center, ship_right_far,
                                   ship_right_front,  player_station_right};

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

int main() {}
