package com.github.zalewa.snekorama.view;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.g2d.BitmapFont;
import com.badlogic.gdx.graphics.g2d.GlyphLayout;
import com.badlogic.gdx.graphics.g2d.Sprite;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.freetype.FreeTypeFontGenerator;
import com.badlogic.gdx.graphics.g2d.freetype.FreeTypeFontGenerator.FreeTypeFontParameter;
import com.badlogic.gdx.graphics.glutils.ShapeRenderer;
import com.badlogic.gdx.graphics.glutils.ShapeRenderer.ShapeType;
import com.badlogic.gdx.utils.Align;
import com.github.zalewa.snekorama.Main;
import com.github.zalewa.snekorama.game.Game;
import com.github.zalewa.snekorama.game.Vec2;
import com.github.zalewa.snekorama.game.actor.Actor;
import com.github.zalewa.snekorama.game.actor.AppleActor;
import com.github.zalewa.snekorama.game.actor.BodyActor;
import com.github.zalewa.snekorama.game.actor.BrickActor;
import com.github.zalewa.snekorama.game.actor.HeadActor;
import com.github.zalewa.snekorama.view.SpriteSheet.GameSprite;

public class View {
	private Main main;
	private SpriteBatch batch;
	private SpriteSheet spriteSheet;
	private ShapeRenderer shape;
	private BitmapFont font;
	private BitmapFont smallerFont;
	private int yOffset;
	private float scoreWidth;
	private float scoreHeight;
	private float scoreY;
	private float scoreSmallFontY;

	public View(Main main) {
		this.main = main;
		this.yOffset = Gdx.graphics.getHeight() - Gdx.graphics.getWidth();
		batch = new SpriteBatch();
		shape = new ShapeRenderer();
		shape.setAutoShapeType(true);
		spriteSheet = new SpriteSheet("sprites.bmp");
		loadFont();
	}

	private void loadFont() {
		FreeTypeFontGenerator generator = new FreeTypeFontGenerator(
			Gdx.files.internal("DejaVuSans.ttf"));
		try {
			FreeTypeFontParameter parameter = new FreeTypeFontParameter();

			parameter.size = 72;
			font = generator.generateFont(parameter);

			parameter.size = 48;
			smallerFont = generator.generateFont(parameter);
		} finally {
			generator.dispose();
		}

		GlyphLayout layout = new GlyphLayout(font, "SCORE:");
		scoreWidth = layout.width;
		scoreHeight = layout.height;
		scoreY = yOffset - ((yOffset - scoreHeight) / 2);

		layout = new GlyphLayout(smallerFont, "0123456789");
		scoreSmallFontY = yOffset - ((yOffset - layout.height) / 2);
	}

	public void render() {
		Gdx.gl.glClearColor(0, 0, 0.5f, 1);
		Gdx.gl.glClear(GL20.GL_COLOR_BUFFER_BIT);

		batch.begin();
		renderActors();
		batch.end();
		renderScore();
	}

	private void renderActors() {
		for (Actor actor : game().getActors()) {
			Vec2 pos = actor.getPosition();
			Vec2 drawPos = new Vec2(
				pos.getX() * spriteSheet.getBreadth(),
				pos.getY() * spriteSheet.getBreadth() + yOffset);
			Sprite sprite = spriteSheet.getSprite(mapActorToSprite(actor));
			float rotate = 0.0f;
			if (!actor.getVelocity().isZero()) {
				rotate = getRotation(actor.getVelocity());
			}
			batch.draw(sprite,
				(float) drawPos.getX(), (float) drawPos.getY(),
				sprite.getOriginX(), sprite.getOriginY(),
				sprite.getWidth(), sprite.getHeight(),
				1.0f, 1.0f, rotate);
		}
	}

	private void renderScore() {
		shape.begin(ShapeType.Filled);
		shape.setColor(0, 0, 0, 1);
		shape.rect(0, 0, Gdx.graphics.getWidth(), yOffset);
		shape.end();

		batch.begin();
		font.draw(batch, "SCORE:", 0, scoreY);
		String scoreText = "" + this.game().getScore();
		if (scoreText.length() < 5) {
			font.draw(batch, scoreText, scoreWidth + 8, scoreY,
				Gdx.graphics.getWidth() - scoreWidth, Align.left, false);
		} else {
			smallerFont.draw(batch, scoreText, scoreWidth + 8, scoreSmallFontY,
					Gdx.graphics.getWidth() - scoreWidth, Align.left, false);
		}
		batch.end();
	}

	private GameSprite mapActorToSprite(Actor actor) {
		if (actor instanceof HeadActor) {
			if (this.game().isAlive()) {
				return GameSprite.HEAD;
			} else {
				return GameSprite.DEAD_HEAD;
			}
		} else if (actor instanceof BodyActor) {
			return GameSprite.BODY;
		} else if (actor instanceof AppleActor) {
			return GameSprite.APPLE;
		} else if (actor instanceof BrickActor) {
			return GameSprite.BRICK;
		} else {
			throw new IllegalStateException("unknown actor class " + actor.getClass().getName());
		}
	}

	private float getRotation(Vec2 velocity) {
		if (velocity.getX() > 0) {
			return 270.0f;
		} else if (velocity.getX() < 0) {
			return 90.0f;
		} else if (velocity.getY() < 0) {
			return 180.0f;
		}
		return 0;
	}

	public void dispose() {
		if (font != null)
			font.dispose();
		if (smallerFont != null)
			smallerFont.dispose();
		if (spriteSheet != null)
			spriteSheet.dispose();
		shape.dispose();
		batch.dispose();
	}

	private Game game() {
		return this.main.getGame();
	}
}
