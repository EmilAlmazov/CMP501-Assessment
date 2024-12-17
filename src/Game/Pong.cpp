#include "Pong.hpp"
#include <iostream>

Pong::Pong(bool game_started, const std::string& clientOrServer)
    : game_started_(game_started), left_paddle_(sf::Vector2f(50.0f, 200.0f)),
      right_paddle_(sf::Vector2f(50.0f, 200.0f)), ball_(sf::Vector2f(50.0f, 50.0f)), ball_speed_(5.0f, 5.0f),
      left_score_(0), right_score_(0)
{
    // https://stackoverflow.com/questions/2340281/check-if-a-string-contains-a-string-in-c
    if (clientOrServer.find("Client") != std::string::npos) {
        window_.create(sf::VideoMode(1920, 1080), "Pong Multiplayer: " + clientOrServer, sf::Style::Default);
        window_.setFramerateLimit(144);
    }

    font_.loadFromFile("arial.ttf");

    left_paddle_.setFillColor(sf::Color::White);
    left_paddle_.setPosition(50, 1080.0f / 2 - 100);

    right_paddle_.setFillColor(sf::Color::White);
    right_paddle_.setPosition(1920 - 100, 1080.0f / 2 - 100);

    ball_.setFillColor(sf::Color::White);
    ball_.setPosition(1920.0f / 2 - 25, 1080.0f / 2 - 25);

    left_score_text_.setFont(font_);
    left_score_text_.setCharacterSize(24);
    left_score_text_.setFillColor(sf::Color::White);
    left_score_text_.setPosition(1920.0f / 2 - 100, 10);
    left_score_text_.setString(std::to_string(left_score_));

    right_score_text_.setFont(font_);
    right_score_text_.setCharacterSize(24);
    right_score_text_.setFillColor(sf::Color::White);
    right_score_text_.setPosition(1920.0f / 2 + 100, 10);
    right_score_text_.setString(std::to_string(right_score_));

    fps_.setFont(font_);
    fps_.setCharacterSize(24);
    fps_.setFillColor(sf::Color::White);
    fps_.setPosition(10, 10);
}

Pong::~Pong() {}
// FUNCTIONS

// === SERVER ===

void Pong::handlePhysics()
{
    ball_.move(ball_speed_);

    // if (game_started_) { ball_.move(ball_speed_); }
    // else {
    //     ball_.setPosition(left_paddle_.getPosition().x + left_paddle_.getSize().x + 2.0f,
    //                       left_paddle_.getPosition().y + left_paddle_.getSize().y / 2 - ball_.getSize().y / 2);
    //     // TODO: Display "waiting for other player to join" text
    // }

    // === Collision physics ===
    // Top and bottom
    if (ball_.getPosition().y < 0 || ball_.getPosition().y > 1080 - 50) { ball_speed_.y *= -1; }
    // Paddles
    handlePaddleCollision(left_paddle_);
    handlePaddleCollision(right_paddle_);

    // Left/Right out of bounds (ie scoring)
    if (ball_.getPosition().x < 0) {
        right_score_++;
        right_score_text_.setString(std::to_string(right_score_));
        game_started_ = false;
        ball_.setPosition(left_paddle_.getPosition().x + left_paddle_.getSize().x + 2.0f, ball_.getPosition().y);
        ball_speed_ = sf::Vector2f(5.0f, 5.0f);
    }
    if (ball_.getPosition().x + ball_.getSize().x > 1920) {
        left_score_++;
        left_score_text_.setString(std::to_string(left_score_));
        game_started_ = false;
        ball_.setPosition(left_paddle_.getPosition().x + left_paddle_.getSize().x + 2.0f, ball_.getPosition().y);
        ball_speed_ = sf::Vector2f(5.0f, 5.0f);
    }
}

void Pong::handlePaddleCollision(sf::RectangleShape& paddle)
{
    if (ball_.getGlobalBounds().intersects(paddle.getGlobalBounds())) {
        ball_speed_.x *= -1;

        // Adjust ball position to prevent it from getting stuck inside the paddle
        if (ball_.getPosition().x < paddle.getPosition().x) {
            ball_.setPosition(paddle.getPosition().x - ball_.getSize().x, ball_.getPosition().y);
        }
        else {
            ball_.setPosition(paddle.getPosition().x + paddle.getSize().x, ball_.getPosition().y);
        }
        ball_speed_.y *= -1;
    }
}

// === CLIENT ===
std::string Pong::getInput(const size_t paddle_index)
{
    // TODO: Enable both clients to use W/S or Up/Down keys (requires getting current focused window)

    // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) { return "Space"; }

    if (paddle_index == 0) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && left_paddle_.getPosition().y > 0) {
            left_paddle_.move(0.0f, -PADDLE_SPEED);
            return "W";
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && left_paddle_.getPosition().y < 1080 - 200) {
            left_paddle_.move(0.0f, PADDLE_SPEED);
            return "S";
        }
    }
    else if (paddle_index == 1) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && right_paddle_.getPosition().y > 0) {
            right_paddle_.move(0.0f, -PADDLE_SPEED);
            return "Up";
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && right_paddle_.getPosition().y < 1080 - 200) {
            right_paddle_.move(0.0f, PADDLE_SPEED);
            return "Down";
        }
    }
    return "";
}

void Pong::render()
{
    window_.clear();

    window_.draw(ball_);
    window_.draw(left_paddle_);
    window_.draw(right_paddle_);
    window_.draw(fps_);
    window_.draw(left_score_text_);
    window_.draw(right_score_text_);

    window_.display();
}
