package com.github.zalewa.snekorama.game;

import java.util.LinkedList;
import java.util.List;

import com.github.zalewa.snekorama.game.actor.Actor;
import com.github.zalewa.snekorama.game.actor.AppleActor;
import com.github.zalewa.snekorama.game.actor.BodyActor;
import com.github.zalewa.snekorama.game.actor.BrickActor;
import com.github.zalewa.snekorama.game.actor.HeadActor;
import com.github.zalewa.snekorama.view.Audio;

public class Game {
	private static final Vec2 GAME_SIZE = new Vec2(16, 16);
	private static final int APPLE_WORTH = 117;

	private State state;

	public Game() {
	}

	public void newGame() {
		this.state = new State();
	}

	public void tick() {
		if (!state.isAlive) {
			return;
		}

		if (state.nextVelocity != null) {
			state.playerHead.setVelocity(state.nextVelocity);
			state.nextVelocity = null;
		}

		if (!state.playerHead.getVelocity().isZero()) {
			advancePlayer();
		}
	}

	public List<Actor> getActors() {
		return state.board.getActors();
	}

	public int getScore() {
		return state.score;
	}

	public boolean isAlive() {
		return state.isAlive;
	}

	public void changePlayerVelocity(Vec2 velocity) {
		if (state.playerBody.size() > 0) {
			// Forbid reverse when the player has body.
			if (isReverseVelocity(velocity, state.playerHead.getVelocity())) {
				velocity = null;
			}
		}
		state.nextVelocity = velocity;
	}

	private boolean isReverseVelocity(Vec2 velocity, Vec2 velocity2) {
		if (Math.abs(velocity.getX() - velocity2.getX()) >= 2) {
			return true;
		}
		if (Math.abs(velocity.getY() - velocity2.getY()) >= 2) {
			return true;
		}
		return false;
	}

	private void advancePlayer() {
		Vec2 newPos = wrapPos(state.playerHead.getPosition().add(state.playerHead.getVelocity()));
		Actor collidee = state.board.getActorAtPos(newPos);
		if (collidee != null) {
			if (collidee instanceof AppleActor) {
				score();
				movePlayerTo(newPos);
			} else if (collidee instanceof BrickActor || collidee instanceof BodyActor) {
				killPlayer();
			}
		} else {
			movePlayerTo(newPos);
		}
	}

	private void movePlayerTo(Vec2 pos) {
		Audio.get().getPop().play();
		Vec2 prevPos = state.playerHead.getPosition();
		state.playerHead.setPosition(pos);
		if (state.playerBody.size() < state.playerLength - 1) {
			BodyActor body = new BodyActor(prevPos);
			state.playerBody.add(0, body);
			state.board.addActor(body);
		} else if (state.playerBody.size() > 0) {
			int tailIdx = state.playerBody.size() - 1;
			BodyActor tail = state.playerBody.get(tailIdx);
			state.playerBody.remove(tailIdx);
			tail.setPosition(prevPos);
			state.playerBody.add(0, tail);
		}
	}

	private void score() {
		Audio.get().getScore().play();
		state.score += APPLE_WORTH;
		state.playerLength += 1;
		AppleActor apple = (AppleActor) state.board.findActor(actor -> actor instanceof AppleActor);
		apple.setPosition(state.board.randomFreePos());
	}

	private void killPlayer() {
		Audio.get().getSmash().play();
		state.isAlive = false;
	}

	private Vec2 wrapPos(Vec2 pos) {
		if (pos.getX() < 0) {
			pos = new Vec2(state.board.getWidth() - 1, pos.getY());
		} else if (pos.getX() >= state.board.getWidth()) {
			pos = new Vec2(0, pos.getY());
		}
		if (pos.getY() < 0) {
			pos = new Vec2(pos.getX(), state.board.getHeight() - 1);
		} else if (pos.getY() >= state.board.getHeight()) {
			pos = new Vec2(pos.getX(), 0);
		}
		return pos;
	}

	private class State {
		public Vec2 nextVelocity;
		Board board = Board.createRandomBoard(GAME_SIZE, 4);
		int score;
		int playerLength = 1;
		HeadActor playerHead;
		List<BodyActor> playerBody = new LinkedList<>();
		boolean isAlive = true;

		State() {
			playerHead = (HeadActor) board.findActor(actor -> actor instanceof HeadActor);
		}
	}
}
