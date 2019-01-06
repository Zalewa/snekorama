package com.github.zalewa.snekorama.view;

import java.util.Map;
import java.util.TreeMap;

import com.badlogic.gdx.files.FileHandle;
import com.badlogic.gdx.graphics.Pixmap;
import com.badlogic.gdx.graphics.Pixmap.Format;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.Sprite;

/**
 * This class only supports the sprites.bmp sheet.
 */
public class SpriteSheet {
	public enum GameSprite {
		HEAD, DEAD_HEAD, BODY, APPLE, BRICK
	}

	final static int SPRITE_SIZE = 32;

	private Map<GameSprite, Sprite> sheet = new TreeMap<>();
	private Texture texture;

	public SpriteSheet(String resource) {
		Pixmap sheet = null, sprites = null;

		sheet = new Pixmap(new FileHandle(resource));
		sprites = new Pixmap(sheet.getWidth(), sheet.getHeight(), Format.RGBA8888);
		try {
			// Load by converting all black pixels to transparent.
			for (int x = 0; x < sheet.getWidth(); ++x) {
				for (int y = 0; y < sheet.getHeight(); ++y) {
					int color = sheet.getPixel(x, y);
					if ((color & 0xffffff00) == 0) {
						// black to transparent
						sprites.drawPixel(x, y, 0);
					} else {
						sprites.drawPixel(x, y, color);
					}
				}
			}
			this.texture = new Texture(sprites);
			loadRegions();
		} catch (RuntimeException e) {
			this.dispose();
			throw e;
		} finally {
			if (sheet != null)
				sheet.dispose();
			if (sprites != null)
				sprites.dispose();
		}
	}

	public void dispose() {
		if (this.texture != null)
			this.texture.dispose();
	}

	public Texture getTexture() {
		return this.texture;
	}

	public Sprite getSprite(GameSprite sprite) {
		return this.sheet.get(sprite);
	}

	private void loadRegions() {
		addSheet(GameSprite.BODY, 0, 0);
		addSheet(GameSprite.HEAD, 1, 0);
		addSheet(GameSprite.DEAD_HEAD, 2, 0);
		addSheet(GameSprite.APPLE, 0, 1);
		addSheet(GameSprite.BRICK, 1, 1);
	}

	private void addSheet(GameSprite klass, int x, int y) {
		Sprite sprite = new Sprite(
			texture, x * SPRITE_SIZE, y * SPRITE_SIZE,
			SPRITE_SIZE, SPRITE_SIZE);
		this.sheet.put(klass, sprite);
	}

	public int getBreadth() {
		return SPRITE_SIZE;
	}
}
