//
// Created by andres on 5/10/25.
//

// timer.cpp
#include "timer.h"

#include <atomic>
#include <condition_variable>
#include <cmath>
#include <mutex>
#include <random>

namespace cc::time {

// -------------------------------
// Stopwatch
// -------------------------------
Stopwatch Stopwatch::start_new() noexcept {
    Stopwatch sw;
    sw.start();
    return sw;
}

Stopwatch::Stopwatch() noexcept = default;

void Stopwatch::start() noexcept {
    if (running_) return;
    running_ = true;
    start_   = SteadyClock::now();
}

void Stopwatch::stop() noexcept {
    if (!running_) return;
    const auto now = SteadyClock::now();
    acc_ += std::chrono::duration_cast<Millis>(now - start_);
    running_ = false;
}

void Stopwatch::reset() noexcept {
    running_ = false;
    acc_     = Millis{0};
    start_   = {};
}

bool Stopwatch::running() const noexcept {
    return running_;
}

Millis Stopwatch::elapsed() const noexcept {
    if (!running_) return acc_;
    const auto now   = SteadyClock::now();
    const auto delta = std::chrono::duration_cast<Millis>(now - start_);
    return acc_ + delta;
}

// -------------------------------
// Deadline
// -------------------------------
Deadline::Deadline(Millis timeout) noexcept
    : deadline_{SteadyClock::now() + timeout} {}

Deadline::Deadline(SteadyClock::time_point tp) noexcept
    : deadline_{tp} {}

bool Deadline::expired() const noexcept {
    return SteadyClock::now() >= deadline_;
}

Millis Deadline::remaining() const noexcept {
    const auto now = SteadyClock::now();
    if (now >= deadline_) return Millis{0};
    return std::chrono::duration_cast<Millis>(deadline_ - now);
}

SteadyClock::time_point Deadline::when() const noexcept {
    return deadline_;
}

void Deadline::extend(Millis delta) noexcept {
    deadline_ += delta;
}

// -------------------------------
// Backoff
// -------------------------------

static inline std::uint64_t splitmix64(std::uint64_t x) noexcept {
    // PRNG simple para generar estado a partir de la semilla
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

Backoff::Backoff(BackoffPolicy p, std::uint64_t seed) noexcept
    : pol_(p),
      rng_state_(seed ? seed : splitmix64(static_cast<std::uint64_t>(
                    std::chrono::steady_clock::now().time_since_epoch().count()))),
      next_attempt_(1) {}

int Backoff::attempt() const noexcept {
    return next_attempt_;
}

Millis Backoff::next_delay() noexcept {
    // Si superamos max_attempts, devolvemos max_delay (la capa superior decidirá abortar).
    const int k = (next_attempt_ <= 0 ? 1 : next_attempt_);
    if (pol_.max_attempts > 0 && k > pol_.max_attempts) {
        return pol_.max_delay;
    }

    // Exponencial: base * factor^(k-1)
    const double base_ms = static_cast<double>(pol_.base.count());
    double raw_ms = base_ms * std::pow(pol_.factor <= 1.0 ? 1.0 : pol_.factor, static_cast<double>(k - 1));

    // Clamp al máximo
    const double max_ms = static_cast<double>(pol_.max_delay.count());
    if (raw_ms > max_ms) raw_ms = max_ms;

    // Jitter uniforme en ±jitter_pct
    double jittered_ms = raw_ms;
    if (pol_.jitter_pct > 0.0) {
        // RNG simple: xorshift-like con splitmix64
        rng_state_ = splitmix64(rng_state_);
        // Mapear a [0,1)
        const double u01 = (rng_state_ >> 11) * (1.0 / 9007199254740992.0); // 53 bits -> [0,1)
        const double span = pol_.jitter_pct;
        const double jitter = (u01 * 2.0 - 1.0) * span; // [-span, +span]
        jittered_ms = raw_ms * (1.0 + jitter);
        if (jittered_ms < 0.0)    jittered_ms = 0.0;
        if (jittered_ms > max_ms) jittered_ms = max_ms;
    }

    // Avanzar contador
    ++next_attempt_;

    // Convertir a Millis
    const auto ms = static_cast<long long>(std::llround(jittered_ms));
    return Millis{ms};
}

void Backoff::reset() noexcept {
    next_attempt_ = 1;
}

// -------------------------------
// Cancelación y sleeps
// -------------------------------
struct CancellationSource::State {
    std::atomic<bool> cancelled{false};
    std::mutex        m;
    std::condition_variable cv;
};

CancellationSource::CancellationSource()
    : state_(new State()) {}

void CancellationSource::cancel() noexcept {
    if (!state_) return;
    state_->cancelled.store(true, std::memory_order_relaxed);
    std::lock_guard<std::mutex> lk(state_->m);
    state_->cv.notify_all();
}

CancellationToken CancellationSource::token() const noexcept {
    return CancellationToken{state_};
}

bool CancellationToken::is_cancelled() const noexcept {
    if (!impl_) return false;
    auto* s = static_cast<CancellationSource::State*>(impl_);
    return s->cancelled.load(std::memory_order_relaxed);
}

bool sleep_until(SteadyClock::time_point tp, const CancellationToken* ct) noexcept {
    // Si no hay token, usamos un CV local para esperar de forma eficiente.
    if (!ct || !ct->impl_) {
        std::mutex m;
        std::unique_lock<std::mutex> lk(m);
        std::condition_variable cv;
        // wait_until con steady_clock
        while (SteadyClock::now() < tp) {
            const auto status = cv.wait_until(lk, tp);
            (void)status; // spurious wakeups son inofensivos aquí
        }
        return true;
    }

    auto* s = static_cast<CancellationSource::State*>(ct->impl_);
    std::unique_lock<std::mutex> lk(s->m);

    while (true) {
        if (s->cancelled.load(std::memory_order_relaxed)) {
            return false; // cancelado
        }
        if (SteadyClock::now() >= tp) {
            return true;  // llegó la hora
        }
        s->cv.wait_until(lk, tp);
        // loop: revisa cancelación o tiempo
    }
}

bool sleep_for(Millis d, const CancellationToken* ct) noexcept {
    return sleep_until(SteadyClock::now() + d, ct);
}

} // namespace cc::time
