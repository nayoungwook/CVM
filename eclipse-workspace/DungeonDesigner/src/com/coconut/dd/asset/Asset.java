package com.coconut.dd.asset;

import dev.suback.marshmallow.object.MSSprite;

import java.io.IOException;

public class Asset {
    private static Asset instance = null;

    private Asset() {
        System.out.println("loading assets...");
        try {
            loadAssets();
        } catch (IOException e) {
            e.printStackTrace();
        }
        System.out.println("assets loaded successfully.");
    }

    public void loadAssets() throws IOException {

    }

    public static Asset getInstance() {
        if (instance == null) {
            synchronized (Asset.class) {
                if (instance == null) {
                    instance = new Asset();
                }
            }
        }
        return instance;
    }
}
