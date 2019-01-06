package com.github.zalewa.snekorama;

import com.badlogic.gdx.ApplicationAdapter;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input.Keys;
import com.badlogic.gdx.controllers.Controller;
import com.badlogic.gdx.controllers.Controllers;
import com.github.zalewa.snekorama.game.Game;
import com.github.zalewa.snekorama.game.TickTock;
import com.github.zalewa.snekorama.game.Vec2;
import com.github.zalewa.snekorama.view.Audio;
import com.github.zalewa.snekorama.view.View;

public class Main extends ApplicationAdapter {
	private static final int SUFFICIENTLY_LARGE_AMOUNT_OF_BUTTONS = 20;
	private Game game;
	private View view;
	private TickTock tickTock = new TickTock(10);

	@Override
	public void create() {
		view = new View(this);
		game = new Game();
		resetGame();
	}

	@Override
	public void render() {
		handleInput();
		handleJoystick();
		if (tickTock.tickDown(Gdx.graphics.getDeltaTime())) {
			game.tick();
		}
		view.render();
	}

	@Override
	public void dispose() {
		view.dispose();
		Audio.get().dispose();
	}

	public Game getGame() {
		return this.game;
	}

	private void resetGame() {
		game.newGame();
		tickTock.reset();
	}

	private void handleInput() {
		if (Gdx.input.isKeyPressed(Keys.LEFT)) {
			this.game.changePlayerVelocity(new Vec2(-1, 0));
		} else if (Gdx.input.isKeyPressed(Keys.RIGHT)) {
			this.game.changePlayerVelocity(new Vec2(1, 0));
		} else if (Gdx.input.isKeyPressed(Keys.DOWN)) {
			this.game.changePlayerVelocity(new Vec2(0, -1));
		} else if (Gdx.input.isKeyPressed(Keys.UP)) {
			this.game.changePlayerVelocity(new Vec2(0, 1));
		} else if (Gdx.input.isKeyPressed(Keys.SPACE) || Gdx.input.isKeyPressed(Keys.ENTER)) {
			if (!this.game.isAlive()) {
				resetGame();
			}
		} else if (Gdx.input.isKeyPressed(Keys.ESCAPE)) {
			Gdx.app.exit();
		}
	}

	private void handleJoystick() {
		for (Controller controller : Controllers.getControllers()) {
			float yaxis = controller.getAxis(0);
			float xaxis = controller.getAxis(1);
			if (xaxis < -0.5f) {
				this.game.changePlayerVelocity(new Vec2(-1, 0));
			} else if (xaxis > 0.5f) {
				this.game.changePlayerVelocity(new Vec2(1, 0));
			}
			if (yaxis < -0.5f) {
				this.game.changePlayerVelocity(new Vec2(0, 1));
			} else if (yaxis > 0.5f) {
				this.game.changePlayerVelocity(new Vec2(0, -1));
			}
			gameReset:
			if (!this.game.isAlive()) {
				for (int i = 0; i < SUFFICIENTLY_LARGE_AMOUNT_OF_BUTTONS; ++i) {
					if (controller.getButton(i)) {
						resetGame();
						break gameReset;
					}
				}
			}
		}
	}
}
