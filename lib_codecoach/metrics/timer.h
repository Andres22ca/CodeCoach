//
// Created by andres on 5/10/25.
//


// Utilidades de tiempo para CodeCoach: Stopwatch, Deadline, Backoff y cancelación.
#ifndef LIB_CODECOACH_TIMER_H
#define LIB_CODECOACH_TIMER_H

#include <chrono>
#include <cstdint>

namespace cc::time {

// Tipos base
using SteadyClock = std::chrono::steady_clock;
using Millis      = std::chrono::milliseconds;
using Micros      = std::chrono::microseconds;

//medir duracion
class Stopwatch {
public:
    static Stopwatch start_new() noexcept;

    Stopwatch() noexcept;
    void   start() noexcept;
    void   stop() noexcept;
    void   reset() noexcept;
    bool   running() const noexcept;
    Millis elapsed() const noexcept; // si está corriendo, mide hasta "ahora"

private:
    SteadyClock::time_point start_{};
    Millis                  acc_{0};
    bool                    running_{false};
};

//deadline
class Deadline {
public:
    explicit Deadline(Millis timeout) noexcept;                // now + timeout
    explicit Deadline(SteadyClock::time_point tp) noexcept;    // absoluto

    bool   expired() const noexcept;
    Millis remaining() const noexcept; // 0 si ya venció
    SteadyClock::time_point when() const noexcept;
    void   extend(Millis delta) noexcept;

private:
    SteadyClock::time_point deadline_;
};


// Backoff exponencial con jitter
struct BackoffPolicy {
    Millis base{100};      // primer delay
    double factor{2.0};    // multiplicador exponencial (> 1.0)
    Millis max_delay{5000};
    double jitter_pct{0.2}; // 0.2 => ±20%
    int    max_attempts{5}; // límite de intentos (≥ 1)
};

class Backoff {
public:
    explicit Backoff(BackoffPolicy p, std::uint64_t seed = 0) noexcept;

    // attempt() es el índice del PRÓXIMO intento (1-based)
    int    attempt() const noexcept;
    Millis next_delay() noexcept; // aplica exponencial + jitter; avanza contador
    void   reset() noexcept;

private:
    BackoffPolicy          pol_{};
    std::uint64_t          rng_state_{0};
    int                    next_attempt_{1}; // 1-based
};


// Cancelación ligera (token/source)

class CancellationToken {
public:
    CancellationToken() noexcept = default;
    explicit CancellationToken(void* impl) noexcept : impl_(impl) {}

    bool is_cancelled() const noexcept;

private:
    void* impl_{nullptr}; // puntero opaco a estado compartido
    friend class CancellationSource;

    friend bool sleep_until(SteadyClock::time_point tp, const CancellationToken* ct) noexcept;
    friend bool sleep_for(Millis d, const CancellationToken* ct) noexcept;
};

class CancellationSource {
public:
    CancellationSource();
    void cancel() noexcept;
    CancellationToken token() const noexcept;


    struct State;
private:
    // PIMPL opaco; no exponemos <mutex> ni <condition_variable> aquí

    State* state_;

};



// Utilidades de espera (cancelables)

// Devuelve true si completó la espera; false si se canceló antes.
bool sleep_for(Millis d, const CancellationToken* ct = nullptr) noexcept;

// Devuelve true si alcanzó el tiempo objetivo; false si se canceló antes.
bool sleep_until(SteadyClock::time_point tp, const CancellationToken* ct = nullptr) noexcept;

} // namespace cc::time

#endif // LIB_CODECOACH_TIMER_H
