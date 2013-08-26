import fisica.*;

FWorld world;

void physics_setup()
{
  Fisica.init(this);

  world = new FWorld();
  world.setGravity(0, 800);
  world.setEdgesRestitution(0.5);
  physics_clear();
}

void physics_clear()
{
  ArrayList bodies = world.getBodies();
  int nBodies = bodies.size();
  for (int i=0;i<nBodies;i++)
    world.remove((FBody)bodies.get(i));
  world.setEdges();
  world.remove(world.left);
  world.remove(world.right);
  world.remove(world.top);    
}

void physics_draw()
{
  world.step();
  world.draw(this);
}

void physics_update_blob(vBlob blob)
{
  int nPts = blob.points.size();
  if (nPts > 0)
  {
    if (blob.poly == null)
    {
      blob.poly = new FPoly();
      blob.poly.setStrokeWeight(3);
      blob.poly.setFill(random(255), random(255), random(255));
      blob.poly.setDensity(10);
      blob.poly.setRestitution(0.5);
      for (int i=0;i<nPts;i++)
      {
        PVector pos = (PVector)blob.points.get(i);
        blob.poly.vertex(pos.x, pos.y);
      }
      world.add(blob.poly);    
    } 
  }
}
