#pragma once

#include <SFML/Graphics.hpp>
#include <atomic>

constexpr float PADDLE_SPEED = 5.0f;

class Pong {
public:
    explicit Pong(bool game_started, const std::string& clientOrServer);

    ~Pong();

    // === GETTERS ===
    sf::RenderWindow& getWindow() { return window_; }
    bool windowIsOpen() const { return window_.isOpen(); }

    sf::RectangleShape& getLeftPaddle() { return left_paddle_; }
    sf::RectangleShape& getRightPaddle() { return right_paddle_; }
    sf::RectangleShape& getBall() { return ball_; }
    sf::Vector2f getBallSpeed() const { return ball_speed_; }
    int getLeftScore() const { return left_score_; }
    int getRightScore() const { return right_score_; }
    sf::Text& getLeftScoreText() { return left_score_text_; }
    sf::Text& getRightScoreText() { return right_score_text_; }

    bool getGameStarted() const { return game_started_; }

    // === SETTERS ===
    void setLeftPaddlePosition(const sf::Vector2f& position) { left_paddle_.setPosition(position); }
    void setRightPaddlePosition(const sf::Vector2f& position) { right_paddle_.setPosition(position); }
    void setBallPosition(const sf::Vector2f& position) { ball_.setPosition(position); }
    void setBallSpeed(const sf::Vector2f& speed) { ball_speed_ = speed; }
    void setLeftScore(const int score) { left_score_ = score; }
    void setRightScore(const int score) { right_score_ = score; }

    void setGameStarted(const bool game_started) const { game_started_ = game_started; }

    // SERVER
    void handlePhysics();

    // CLIENT
    std::string getInput(size_t);
    void render();

private:
    // Physics
    void handlePaddleCollision(sf::RectangleShape& paddle);

    // ------------------------------------------------------------------------

    // === VARIABLES ===
    // Game logic
    sf::RenderWindow window_;
    bool& game_started_;

    // Objects
    sf::RectangleShape left_paddle_;
    sf::RectangleShape right_paddle_;
    sf::RectangleShape ball_;
    sf::Vector2f ball_speed_;

    // UI
    sf::Font font_;
    int left_score_;
    int right_score_;
    sf::Text left_score_text_;
    sf::Text right_score_text_;
    sf::Text fps_;
    sf::Clock clock_;
    sf::Time time_;
};
