package com.coconut.dd;

import dev.suback.marshmallow.MSDisplay;
import dev.suback.marshmallow.object.shape.MSRender;
import dev.suback.marshmallow.state.MSState;

import java.awt.*;

public class Game implements MSState {
    @Override
    public void init() {

    }

    @Override
    public void update() {

    }

    @Override
    public void render() {
        MSRender.setColor(new Color(20, 20, 20));
        MSRender.renderUIRect((float) MSDisplay.width / 2, (float) MSDisplay.height / 2, MSDisplay.width, MSDisplay.height);
    }
}
