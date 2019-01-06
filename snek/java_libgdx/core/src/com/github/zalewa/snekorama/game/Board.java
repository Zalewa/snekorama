package com.github.zalewa.snekorama.game;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.function.Predicate;
import java.util.stream.Collectors;

import com.badlogic.gdx.math.MathUtils;
import com.github.zalewa.snekorama.game.actor.Actor;
import com.github.zalewa.snekorama.game.actor.AppleActor;
import com.github.zalewa.snekorama.game.actor.BrickActor;
import com.github.zalewa.snekorama.game.actor.HeadActor;

public class Board {
	private List<Actor> actors = new ArrayList<>();
	private Vec2 size;

	public static Board createRandomBoard(Vec2 size, int numBricks) {
		Board board = new Board(size);

		// Place bricks.
		for (int i = 0; i < numBricks; ++i) {
			Vec2 pos = board.randomFreePos();
			board.addActor(new BrickActor(pos));
		}

		// Place head.
		Vec2 headPos = board.randomFreePos();
		board.addActor(new HeadActor(headPos));

		// Place apple.
		Vec2 applePos = board.randomFreePos();
		board.addActor(new AppleActor(applePos));

		return board;
	}

	public Board(Vec2 size) {
		this.size = size;
	}

	public void addActor(Actor actor) {
		this.actors.add(actor);
	}

	public void removeActor(Actor actor) {
		this.actors.remove(actor);
	}

	public List<Actor> getActors() {
		return Collections.unmodifiableList(this.actors);
	}

	public Actor findActor(Predicate<Actor> predicate) {
		return this.actors.stream().filter(predicate).findFirst().orElse(null);
	}

	public List<Actor> findAllActors(Predicate<Actor> predicate) {
		return this.actors.stream().filter(predicate).collect(Collectors.toList());
	}

	public Actor getActorAtPos(Vec2 pos) {
		return findActor(a -> a.getPosition().equals(pos));
	}

	public int getWidth() {
		return this.size.getX();
	}

	public int getHeight() {
		return this.size.getY();
	}

	public Vec2 randomFreePos() {
		Vec2 pos;
		do {
			pos = new Vec2(
				MathUtils.random(0, getWidth() - 1),
				MathUtils.random(0, getHeight() - 1));
		} while(getActorAtPos(pos) != null);
		return pos;
	}
}
