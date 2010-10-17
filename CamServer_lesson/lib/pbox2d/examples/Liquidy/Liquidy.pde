// The Nature of Code
// <http://www.shiffman.net/teaching/nature>
// Spring 2010
// PBox2D example

// Box2D particle system example

import pbox2d.*;
import org.jbox2d.collision.shapes.*;
import org.jbox2d.common.*;
import org.jbox2d.dynamics.*;



// A reference to our box2d world
PBox2D box2d;

// A list we'll use to track fixed objects
ArrayList boundaries;

// A list for all particle systems
ArrayList systems;

void setup() {
  size(400,300);
  smooth();

  // Initialize box2d physics and create the world
  box2d = new PBox2D(this);
  box2d.createWorld();

  // We are setting a custom gravity
  box2d.setGravity(0, -20);

  // Create ArrayLists	
  systems = new ArrayList();
  boundaries = new ArrayList();

  // Add a bunch of fixed boundaries
  boundaries.add(new Boundary(50,100,300,5,-0.3f));
  boundaries.add(new Boundary(250,175,300,5,0.5f));

}

void draw() {
  background(255);

  // We must always step through time!
  box2d.step();

  // Run all the particle systems
  for (int i = 0; i < systems.size(); i++) {
    ParticleSystem system = (ParticleSystem) systems.get(i);
    system.run();

    int n = (int) random(0,2);
    system.addParticles(n);
  }

  // Display all the boundaries
  for (int i = 0; i < boundaries.size(); i++) {
    Boundary wall = (Boundary) boundaries.get(i);
    wall.display();
  }
}


void mousePressed() {
  // Add a new Particle System whenever the mouse is clicked
  systems.add(new ParticleSystem(0, new PVector(mouseX,mouseY)));
}





