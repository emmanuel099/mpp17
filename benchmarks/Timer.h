#pragma once

#include <chrono>
#include <stdexcept>

template <typename Clock>
class Timer
{
  private:
    enum class State { Started, Stopped };

  public:
    Timer()
        : m_state(State::Stopped)
        , m_start(Clock::now())
        , m_end(m_start)
    {
    }

    void start()
    {
        if (m_state != State::Stopped) {
            throw std::logic_error("timer must be in state `stopped`");
        }

        m_start = Clock::now();
        m_state = State::Started;
    }

    void stop()
    {
        if (m_state != State::Started) {
            throw std::logic_error("timer must be in state `started`");
        }

        m_end = Clock::now();
        m_state = State::Stopped;
    }

    std::chrono::nanoseconds elapsed() const
    {
        if (m_state != State::Stopped) {
            throw std::logic_error("timer must be in state `stopped`");
        }

        return m_end - m_start;
    }

  private:
    State m_state;
    typename Clock::time_point m_start;
    typename Clock::time_point m_end;
};
