#include "anim.hpp"

Animation::Animation()
        : m_texture(nullptr), m_frameTime(sf::seconds(0.1f)), m_currentFrame(0), m_isPlaying(false), m_startFrame(0), m_endFrame(0) {}

void Animation::addFrame(const sf::IntRect& frame) {
    m_frames.push_back(frame);
}

void Animation::setSpriteSheet(const sf::Texture& texture) {
    m_texture = &texture;
}

void Animation::setFrameTime(sf::Time time) {
    m_frameTime = time;
}

void Animation::play() {
    m_isPlaying = true;
    m_currentFrame = m_startFrame;
    m_currentTime = sf::Time::Zero;
}

void Animation::stop() {
    m_isPlaying = false;
}

void Animation::update(sf::Time deltaTime) {
    if (m_isPlaying) {
        m_currentTime += deltaTime;
        if (m_currentTime >= m_frameTime) {
            m_currentTime = sf::microseconds(m_currentTime.asMicroseconds() % m_frameTime.asMicroseconds());
            if (m_currentFrame + 1 <= m_endFrame) {
                m_currentFrame++;
            } else {
                stop();
            }
        }
    }
}

const sf::Texture* Animation::getSpriteSheet() const {
    return m_texture;
}

sf::IntRect Animation::getFrame() const {
    return m_frames[m_currentFrame];
}

sf::IntRect Animation::getFrame(std::size_t frameIndex) const {
    return m_frames[frameIndex];
}

std::size_t Animation::getCurrentFrameIndex() const {
    return m_currentFrame;
}

bool Animation::isPlaying() const {
    return m_isPlaying;
}

void Animation::setSegment(std::size_t startFrame, std::size_t endFrame) {
    m_startFrame = startFrame;
    m_endFrame = endFrame;
}

std::size_t Animation::getEndFrame() const {
    return m_endFrame;
}
