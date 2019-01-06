package com.github.zalewa.snekorama.desktop;

import com.badlogic.gdx.backends.lwjgl.LwjglApplication;
import com.badlogic.gdx.backends.lwjgl.LwjglApplicationConfiguration;
import com.github.zalewa.snekorama.Main;

public class DesktopLauncher {
	public static void main (String[] arg) {
		LwjglApplicationConfiguration config = new LwjglApplicationConfiguration();
		config.width = 512;
		config.height = 640;
		config.title = "Snek Java LibGDX";
		new LwjglApplication(new Main(), config);
	}
}
