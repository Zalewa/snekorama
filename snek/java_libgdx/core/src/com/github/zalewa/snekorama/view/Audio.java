package com.github.zalewa.snekorama.view;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.audio.Sound;

public class Audio {
	private static Audio instance;

	private Sound pop;
	private Sound smash;
	private Sound score;

	public static Audio get() {
		if (instance == null)
			instance = new Audio();
		return instance;
	}

	private Audio() {
		pop = Gdx.audio.newSound(Gdx.files.internal("pop.wav"));
		smash = Gdx.audio.newSound(Gdx.files.internal("smash.wav"));
		score = Gdx.audio.newSound(Gdx.files.internal("score.wav"));
	}

	public void dispose() {
		pop.dispose();
		smash.dispose();
		score.dispose();
	}

	public Sound getPop() {
		return pop;
	}

	public Sound getSmash() {
		return smash;
	}

	public Sound getScore() {
		return score;
	}
}
