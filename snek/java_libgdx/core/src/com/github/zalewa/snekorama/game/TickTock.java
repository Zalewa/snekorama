package com.github.zalewa.snekorama.game;

public class TickTock {
	private float startCountdown = 0;
	private float countdown = 0;

	public TickTock(int ticksPerSecond) {
		this.startCountdown = this.countdown = 1.0f / ticksPerSecond;
	}

	public boolean tickDown(float deltaMs) {
		this.countdown -= deltaMs;
		if (this.countdown <= 0.0f) {
			this.countdown = this.startCountdown + this.countdown;
			return true;
		} else {
			return false;
		}
	}

	public void reset() {
		this.countdown = this.startCountdown;
	}
}
