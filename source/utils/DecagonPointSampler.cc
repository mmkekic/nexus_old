// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: Justo Martín-Albo <jmalbos@ific.uv.es>
//           Francesc Monrabal <francesc.monrabal@ific.uv.es>
//  Created: 13 May 2010
//
//  Copyright (c) 2010, 2011 NEXT Collaboration
//
//  Updated based on "HexagonPointSampler.cc"  ==>>>  "DecagonPointSampler.cc"
//  for the new DEMO++  ACTIVE  having 10 walls polygon instead of the former 6 walls.
//  Draft by: Ruth Weiss Babai <ruty.wb@gmail.com>
//  Date:     10 Apr 2020
//  ************   Need to be chacked   ************
// ----------------------------------------------------------------------------

//#include "HexagonPointSampler.h"
#include "DecagonPointSampler.h"

#include <Randomize.hh>
#include <G4RunManager.hh>

#include "CLHEP/Units/PhysicalConstants.h"

#include <vector>


namespace nexus {

  using namespace CLHEP;


  //HexagonPointSampler::HexagonPointSampler
  DecagonPointSampler::DecagonPointSampler
  (G4double apothem, G4double length, G4double thickness,
   G4ThreeVector origin, G4RotationMatrix* rotation):
    _length(length), _apothem(apothem),  _thickness(thickness),
    _origin(origin), _rotation(rotation)
  {
    //_radius = _apothem / cos(pi/6.);
    _radius = _apothem / cos(pi/10.);
  }



  //void HexagonPointSampler::TesselateWithFixedPitch
  void DecagonPointSampler::TesselateWithFixedPitch
  (G4double pitch, std::vector<G4ThreeVector>& vpos)
  {
    //G4double cell_radius = pitch / sqrt(3.);
    //G4double cell_apothem = sqrt(3.)/2. * cell_radius;
    G4double cell_radius = pitch / cos(pi/10.) / 2.;
    G4double cell_apothem = cos(pi/10.) * cell_radius;

    //  ????   RUTY    ?????   Why  2./3.   ???   //
    G4int order = floor(2./3. * (_apothem/cell_radius - 1.));

    // Drop the contents of the vector, if any
    vpos.clear();

    PlaceCells(vpos, order, cell_apothem);
  }


  //G4ThreeVector HexagonPointSampler::GenerateVertex(HexagonRegion region)
  G4ThreeVector DecagonPointSampler::GenerateVertex(DecagonRegion region)
  {
    G4ThreeVector vertex;
    if (region == INSIDE10) {

      //  ??????   RUTY    ?????
      // Get a random point in a triangular sector of the hexagonal section
      G4ThreeVector point = RandomPointInTriangle();

      // Pick a triangular sector of the hexagon and rotate the
      // previous point accordingly
      //G4int face = floor(G4UniformRand() * 6.);
      //point.rotateZ(face*pi/3.);
      G4int face = floor(G4UniformRand() * 10.);
      point.rotateZ(face*pi/5.);


      // Get a random z coordinate
      point.setZ(RandomLength(-_length/2., _length/2));

      // Transform point to the user's system of reference and return
      vertex =  RotateAndTranslate(point);
    } else if (region == PLANE10) {
       /// Get a random point in a triangular sector of the hexagonal section
      G4ThreeVector point = RandomPointInTriangle();

      /// Pick a triangular sector of the hexagon and rotate the
      /// previous point accordingly
      //G4int face = floor(G4UniformRand() * 6.);
      //point.rotateZ(face*pi/3.);
      G4int face = floor(G4UniformRand() * 10.);
      point.rotateZ(face*pi/5.);

      //point.setZ(RandomLength(-_length/2., -_length/2+100.));
      point.setZ(RandomLength(_length/2.-20., _length/2.));
      vertex =  RotateAndTranslate(point);
    } // else if (region == TRIANGLE) {

    // }
    // // else if (region == TRIANGLE) {

    // //   static G4int index = 0;
    // //   if (index == 0) TriangleWalker();

    // //   return _table_vertices[index];
    // // }
     // Unknown region
    else {
      //G4Exception("[HexagonPointSampler]", "GenerateVertex()", FatalException,
      G4Exception("[DecagonPointSampler]", "GenerateVertex()", FatalException,
		  "Unknown Region!");
    }

    return vertex;
  }



  //G4ThreeVector HexagonPointSampler::RandomPointInTriangle()
  G4ThreeVector DecagonPointSampler::RandomPointInTriangle()
  {
    //G4ThreeVector A(-_radius/2., _radius * cos(pi/6.), 0);
    //G4ThreeVector B( _radius/2., _radius * cos(pi/6.), 0);
    G4ThreeVector A(-_radius/2., _radius * cos(pi/10.), 0);
    G4ThreeVector B( _radius/2., _radius * cos(pi/10.), 0);

    G4double a = G4UniformRand();
    G4double b = G4UniformRand();

    if ((a+b) > 1.) {
      a = 1. - a;
      b = 1. - b;
    }

    G4ThreeVector P;
    P[0] = a * A[0] + b * B[0];
    P[1] = a * A[1] + b * B[1];

    return P;
  }



  //void HexagonPointSampler::PlaceCells(std::vector<G4ThreeVector>& vp,
  void DecagonPointSampler::PlaceCells(std::vector<G4ThreeVector>& vp,
				       G4int order, G4double cell_apothem)
  {
    // Place the central cell
    G4ThreeVector position(0.,0.,0.);
    vp.push_back(RotateAndTranslate(position));

    // Place the rings
    // Notice that the loop starts at order 1 (first ring)
    // because we've placed already the central cell (order-0 element).
    for (G4int n=1; n<=order; n++) {

      // For a ring of order n, there are n independent cells, that is,
      // cells whose position cannot be obtained by rotating one of the
      // other cells in the ring.
      for (G4int i=0; i<n; i++) {

	position.setX(-n * cell_apothem + i * 2. * cell_apothem);
	//position.setY(n* 2. * cell_apothem * cos(pi/6.));
  position.setY(n* 2. * cell_apothem * cos(pi/10.));

	vp.push_back(RotateAndTranslate(position));

	// Rotating 60º this position, we fill the other sides of
  // Rotating 36º (?) this position, we fill the other sides of
	// the honeycomb.
	//for (G4int j=0; j<5; j++)
	  //vp.push_back(RotateAndTranslate(position.rotateZ(pi/3.)));
    for (G4int j=0; j<9; j++)
  	  vp.push_back(RotateAndTranslate(position.rotateZ(pi/5.)));
      }
    }
  }



  //void HexagonPointSampler::TriangleWalker
  void DecagonPointSampler::TriangleWalker
  (G4double radius, G4double binning, G4double z)
  {
    G4double x = 0;
    G4double y = 0;

    while (x < radius/2.) {
      _table_vertices.push_back(G4ThreeVector(x,y,z));
      y += binning;
      if (y > _apothem) {
	x += binning;
	//y = sqrt(3.) * x;     //  2. * cos(pi/6) = sqrt(3)
  y = 2. * cos(pi/10) * x;
      }
    }
  }




  //G4double HexagonPointSampler::RandomRadius(G4double inner, G4double outer)
  G4double DecagonPointSampler::RandomRadius(G4double inner, G4double outer)
  {
    G4double rnd = G4UniformRand();
    G4double r = sqrt((1.-rnd) * inner*inner + rnd * outer*outer);
    return r;
  }



  //G4double HexagonPointSampler::RandomPhi()
  G4double DecagonPointSampler::RandomPhi()
  {
    return (G4UniformRand() * twopi);
  }



  //G4double HexagonPointSampler::RandomLength(G4double min,
  G4double DecagonPointSampler::RandomLength(G4double min,
					     G4double max)
  {
    return (min + G4UniformRand() * (max-min));
  }



  G4ThreeVector
  //HexagonPointSampler::RotateAndTranslate(const G4ThreeVector& position)
  DecagonPointSampler::RotateAndTranslate(const G4ThreeVector& position)
  {
    G4ThreeVector real_position(position);

    // Rotating if needed
    if (_rotation)
      real_position *= (*_rotation);
    // Translating
    real_position += _origin;

    return real_position;
  }




} // end namespace nexus
