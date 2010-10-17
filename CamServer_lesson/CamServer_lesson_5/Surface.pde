// The Nature of Code
// <http://www.shiffman.net/teaching/nature>
// Spring 2010
// PBox2D example

// An uneven surface boundary

class Surface {
  // We'll keep track of all of the surface points
  ArrayList surface;
  Body body;

  public Surface(vBlob src) {
    int n = src.points.size();
    if (n >0)
    {
      surface = new ArrayList();

      // This is what box2d uses to put the surface in its world
      EdgeChainDef edges = new EdgeChainDef();      
      for (int i=0;i<n;i++)
      {
        // The edge point in our window
        PVector pos = (PVector)src.points.get(i);
        Vec2 screenEdge = new Vec2(pos.x,pos.y);
        // We store it for rendering
        surface.add(screenEdge);

        // Convert it to the box2d world and add it to our EdgeChainDef
        Vec2 edge = box2d.screenToWorld(screenEdge);
        edges.addVertex(edge);
      }
      edges.setIsLoop(false);   // We could make the edge a full loop
      edges.friction = 2.0;    // How much friction
      edges.restitution = 0.3; // How bouncy

      // The edge chain is now a body!
      BodyDef bd = new BodyDef();
      bd.position.set(0.0f,0.0f);
      body = box2d.world.createBody(bd);
      body.createShape(edges);
    }
  }

  // This function removes the particle from the box2d world
  void killBody() {
    box2d.destroyBody(body);
  }

  // A simple function to just draw the edge chain as a series of vertex points
  void display() {
    strokeWeight(2);
    stroke(0);
    noFill();
    beginShape();
    for (int i = 0; i < surface.size(); i++) {
      Vec2 v = (Vec2) surface.get(i);
      vertex(v.x,v.y);
    }
    endShape();
  }
}

