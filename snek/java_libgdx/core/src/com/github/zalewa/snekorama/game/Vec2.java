package com.github.zalewa.snekorama.game;

public class Vec2 {
	private int x;
	private int y;

	public Vec2() {
	}

	public Vec2(int x, int y) {
		this.x = x;
		this.y = y;
	}

	public int getX() {
		return x;
	}

	public int getY() {
		return y;
	}

	public boolean isZero() {
		return x == 0 && y == 0;
	}

	public Vec2 add(Vec2 other) {
		return new Vec2(
			this.x + other.getX(),
			this.y + other.getY());
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + x;
		result = prime * result + y;
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		Vec2 other = (Vec2) obj;
		if (x != other.x)
			return false;
		if (y != other.y)
			return false;
		return true;
	}
}
