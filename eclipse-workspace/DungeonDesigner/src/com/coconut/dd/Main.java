package com.coconut.dd;

import dev.suback.marshmallow.MSDisplay;
import dev.suback.marshmallow.state.MSState;

public class Main {
	public static void main(String[] args) {
		MSState.setState(new Game());
		new MSDisplay("title", 1280, 720);
	}
}