package com.github.zalewa.snekorama.game.actor;

import com.github.zalewa.snekorama.game.Vec2;

public class Actor {
	protected Vec2 position = new Vec2();
	protected Vec2 velocity = new Vec2();

	public Vec2 getPosition() {
		return position;
	}

	public void setPosition(Vec2 position) {
		this.position = position;
	}

	public Vec2 getVelocity() {
		return velocity;
	}

	public void setVelocity(Vec2 velocity) {
		this.velocity = velocity;
	}
}
