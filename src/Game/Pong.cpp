#include "Pong.hpp"

#include <iostream>

constexpr float PADDLE_SPEED = 5.0f;

Pong::Pong(std::atomic<bool> game_started, Client *client) : game_started_(game_started), client_(client) {
    commonConstructor();
}

void Pong::commonConstructor() {
    window_.setFramerateLimit(144);

    font_.loadFromFile("arial.ttf");

    left_paddle_.setFillColor(sf::Color::White);
    left_paddle_.setPosition(50, 1080 / 2 - 100);

    right_paddle_.setFillColor(sf::Color::White);
    right_paddle_.setPosition(1920 - 100, 1080 / 2 - 100);

    ball_.setFillColor(sf::Color::White);
    ball_.setPosition(1920 / 2 - 25, 1080 / 2 - 25);

    left_score_text_.setFont(font_);
    left_score_text_.setCharacterSize(24);
    left_score_text_.setFillColor(sf::Color::White);
    left_score_text_.setPosition(1920 / 2 - 100, 10);

    right_score_text_.setFont(font_);
    right_score_text_.setCharacterSize(24);
    right_score_text_.setFillColor(sf::Color::White);
    right_score_text_.setPosition(1920 / 2 + 100, 10);

    fps_.setFont(font_);
    fps_.setCharacterSize(24);
    fps_.setFillColor(sf::Color::White);
    fps_.setPosition(10, 10);
}


Pong::~Pong() {
}

void Pong::run() {
    while (window_.isOpen()) {
        time_ = clock_.restart();
        fps_.setString("FPS: " + std::to_string(static_cast<int>(1.0f / time_.asSeconds())));

        for (auto event = sf::Event(); window_.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window_.close();
            }
        }

        // 1 - Handle player input
        handleInput();

        // 2 - Update game
        updateGame();

        // 3 - Handle networking
        handleNetworking();

        // 4 - Display game
        render();
    }
}

void Pong::handleInput() {
    // Smooth paddle movement
    if (client_->getPaddleIndex() == 0) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && left_paddle_.getPosition().y > 0) {
            left_paddle_.move(0, -PADDLE_SPEED);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && left_paddle_.getPosition().y < 1080 - 200) {
            left_paddle_.move(0, PADDLE_SPEED);
        }
    } else if (client_->getPaddleIndex() == 1) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && right_paddle_.getPosition().y > 0) {
            right_paddle_.move(0, -PADDLE_SPEED);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && right_paddle_.getPosition().y < 1080 - 200) {
            right_paddle_.move(0, PADDLE_SPEED);
        }
    } else {
        std::cerr << "Invalid paddle index: " << client_->getPaddleIndex() << std::endl;
    }
}

void Pong::updateGame() {
    if (game_started_) {
        ball_.move(ball_speed_);
    } else {
        ball_.setPosition(left_paddle_.getPosition().x + left_paddle_.getSize().x + 2.0f,
                          left_paddle_.getPosition().y + left_paddle_.getSize().y / 2 - ball_.getSize().y / 2);
        // TODO: Display "waiting for other player to join" text
    }

    // Collison physics
    if (ball_.getPosition().y < 0 || ball_.getPosition().y > 1080 - 50) {
        ball_speed_.y *= -1;
    }

    handlePaddleCollision(left_paddle_);
    handlePaddleCollision(right_paddle_);

    // Out of bounds
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

void Pong::handlePaddleCollision(sf::RectangleShape &paddle) {
    if (ball_.getGlobalBounds().intersects(paddle.getGlobalBounds())) {
        ball_speed_.x *= -1;

        // Adjust ball position to prevent it from getting stuck inside the paddle
        if (ball_.getPosition().x < paddle.getPosition().x) {
            ball_.setPosition(paddle.getPosition().x - ball_.getSize().x, ball_.getPosition().y);
        } else {
            ball_.setPosition(paddle.getPosition().x + paddle.getSize().x, ball_.getPosition().y);
        }
        ball_speed_.y *= -1;
    }
}

void Pong::handleNetworking() {
    // TODO: send paddle position to server ONLY if it has changed
    if (client_->getPaddleIndex() == 0) {
        client_->sendPaddlePosition(left_paddle_.getPosition());
    } else {
        client_->sendPaddlePosition(right_paddle_.getPosition());
    }

    // TODO: receive ball position and other player's paddle position from server
}


void Pong::render() {
    window_.clear();

    window_.draw(ball_);
    window_.draw(left_paddle_);
    window_.draw(right_paddle_);
    window_.draw(fps_);
    window_.draw(left_score_text_);
    window_.draw(right_score_text_);

    window_.display();
}


// === Getters and Setters ===
sf::Vector2f Pong::getPaddlePosition(const size_t paddle_index) const {
    if (paddle_index == 0) {
        return left_paddle_.getPosition();
    }
    return right_paddle_.getPosition();
}

// Setters
void Pong::setPaddlePosition(const sf::Vector2f &position, const size_t paddle_index) {
    if (paddle_index == 0) {
        left_paddle_.setPosition(position);
    } else {
        right_paddle_.setPosition(position);
    }
}

sf::Vector2f Pong::getBallPosition() const {
    return ball_.getPosition();
}
