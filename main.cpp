#include <math.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <numeric>
#include <tuple>
#include <vector>

using namespace std;

const double kVelocityNoise = 1;  // TODO: tune noise values
const double kYawNoise = 0.005;
const int kSamples = 1000;

const float kRand() {
    return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
}
const float kZRand() { return (kRand() - 0.5); }

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

bool Intersect(Segment line1, Segment line2) {
    // Segment Detection based on:
    // https://stackoverflow.com/questions/3838329/how-can-i-check-if-two-segments-Intersect
    double slope1 = (line1.a.y - line1.b.y) / (line1.a.x - line1.b.x);
    double slope2 = (line2.a.y - line2.b.y) / (line2.a.x - line2.b.x);
    double c1 = line1.a.y - slope1 * line1.a.x;
    double c2 = line2.a.y - slope2 * line2.a.x;
    if (slope1 == slope2) {
        return false;
    }
    double p = (c2 - c1) / (slope1 - slope2);
    if (p < max(min(line1.a.x, line1.b.x), min(line2.a.x, line2.b.x))) {
        return false;
    }
    if (p > min(min(line1.a.x, line1.b.x), min(line2.a.x, line2.b.x))) {
        return false;
    }
    return true;
}

vector<Point> ExpectedTargets(Pose pose) {
    vector<Point> visible_targets;
    for (int i = 0; i < all_targets.size(); i++) {
        bool observable = true;
        Segment sight = {pose.pos, all_targets[i]};
        for (int j = 0; j < segment_map.size(); j++) {
            if (Intersect(sight, segment_map[i])) {
                observable = false;
                break;
            }
        }
        if (observable) {
            visible_targets.push_back(all_targets[i]);
        }
    }
    vector<Point> expected_observations;
    for (int i = 0; i < visible_targets.size(); i++) {
        Point target = visible_targets[i];
        Point rel = {target.x - pose.pos.x, target.y - pose.pos.y};
        double x_prime = rel.x * cos(-pose.yaw) - rel.y * sin(-pose.yaw);
        double y_prime = rel.x * sin(-pose.yaw) + rel.y * cos(-pose.yaw);
        expected_observations.push_back({x_prime, y_prime});
    }
    return expected_observations;
}

double RatePrediction(vector<Point> measured, Pose assessed_position) {
    // vector<Point> expected = ExpectedTargets(assessed_position);
    // TODO: Add rate prediction
    return 0.0;
}

Pose NewPoseWithNoise(Pose previous) {  // TODO: Implement dx to more
                                        // accurately predict new poses.
    Pose new_pose;
    new_pose.pos.x = previous.pos.x + (kZRand() * kVelocityNoise);
    new_pose.pos.y = previous.pos.y + (kZRand() * kVelocityNoise);
    new_pose.yaw = previous.yaw + (kZRand() * kYawNoise);
    return new_pose;
}

vector<Pose> MCLStep(vector<Pose> previous_predictions,
                     vector<Point> measured_targets) {
    // Compare predictions to measured
    double weights[kSamples];
    for (int i = 0; i < kSamples; i++) {
        weights[i] = RatePrediction(measured_targets, previous_predictions[i]);
    }
    // Generate Random predictions around probabilities
    vector<Pose> new_predictions;
    double weights_sum = accumulate(begin(weights), end(weights), 0.0);
    for (int i = 0; i < kSamples; i++) {
        float weight = weights_sum * kRand();
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
    vector<Segment> full_segment_map;
    for (int i = 0; i < segment_map.size(); i++) {
        full_segment_map.push_back(segment_map[i]);
        full_segment_map.push_back(
            {{640 - segment_map[i].a.x, segment_map[i].a.y},
             {640 - segment_map[i].b.x, segment_map[i].b.y}});
        full_segment_map.push_back(
            {{segment_map[i].a.x, 324 - segment_map[i].a.y},
             {segment_map[i].b.x, 324 - segment_map[i].b.y}});
        full_segment_map.push_back(
            {{640 - segment_map[i].a.x, 324 - segment_map[i].a.y},
             {640 - segment_map[i].b.x, 324 - segment_map[i].b.y}});
    }
    vector<Point> full_targets;
    for (int i = 0; i < all_targets.size(); i++) {
        full_targets.push_back(all_targets[i]);
        full_targets.push_back({640 - all_targets[i].x, all_targets[i].y});
        full_targets.push_back({all_targets[i].x, 324 - all_targets[i].y});
        full_targets.push_back(
            {640 - all_targets[i].x, 324 - all_targets[i].y});
    }

    sf::RenderWindow window(sf::VideoMode(648, 324, 32), "Localization");

    vector<sf::Vertex> vertices;
    for (int i = 0; i < full_segment_map.size(); i++) {
        sf::Vertex point_a;
        sf::Vertex point_b;
        point_a.position =
            sf::Vector2f(full_segment_map[i].a.x, full_segment_map[i].a.y);
        point_b.position =
            sf::Vector2f(full_segment_map[i].b.x, full_segment_map[i].b.y);
        vertices.push_back(point_a);
        vertices.push_back(point_b);
    }

    vector<sf::CircleShape> visualized_targets;
    for (int i = 0; i < full_targets.size(); i++) {
        sf::CircleShape circle;
        circle.setRadius(1);
        circle.setFillColor(sf::Color::Green);
        circle.setOutlineThickness(0);
        circle.setPosition(full_targets[i].x, full_targets[i].y);
        visualized_targets.push_back(circle);
    }

    sf::RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(30, 20));
    rectangle.setOutlineColor(sf::Color::Red);
    rectangle.setOutlineThickness(0);
    rectangle.setPosition(10, 20);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            window.clear(sf::Color::Black);
            // cout << "\nSize: " << vertices.size();
            window.draw(&vertices[0], vertices.size(), sf::Lines);
            window.draw(rectangle);
            for (int i = 0; i < visualized_targets.size(); i++) {
                window.draw(visualized_targets[i]);
            }
            window.display();
        }
    }

    return 0;
}
