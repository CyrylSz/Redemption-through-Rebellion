#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <SFML/Graphics.hpp>
#include <vector>

class Animation {
public:
    Animation();

    void addFrame(const sf::IntRect& frame);
    void setSpriteSheet(const sf::Texture& texture);
    void setFrameTime(sf::Time time);

    void play();
    void stop();
    void update(sf::Time deltaTime);

    const sf::Texture* getSpriteSheet() const;
    sf::IntRect getFrame() const;
    sf::IntRect getFrame(std::size_t frameIndex) const;
    std::size_t getCurrentFrameIndex() const;

    bool isPlaying() const;

    void setSegment(std::size_t startFrame, std::size_t endFrame);
    std::size_t getEndFrame() const;

private:
    std::vector<sf::IntRect> m_frames;
    const sf::Texture* m_texture;
    sf::Time m_frameTime;
    sf::Time m_currentTime;
    std::size_t m_currentFrame;
    std::size_t m_startFrame;
    std::size_t m_endFrame;
    bool m_isPlaying;
};

#endif // ANIMATION_HPP
