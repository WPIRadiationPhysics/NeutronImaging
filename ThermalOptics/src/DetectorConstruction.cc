#include "DetectorConstruction.hh"
#include "UImessenger.hh"
#include "Messenger.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"

G4ThreadLocal 
G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = 0; 

DetectorConstruction::DetectorConstruction()
 : G4VUserDetectorConstruction(),
   fDetector(0),
   fCheckOverlaps(true) {

     // Instantiate UI messenger
     fUImessenger = new UImessenger(this);

     // Initialize model parameters
     fLDratio = 1; // Length-Diameter ratio of collimator

     // Acquire material data
     DefineMaterials();
   }

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct() {

  // Define geometric volumes
  return DefineVolumes();
}

void DetectorConstruction::ModelLDConfiguration(G4int LD_i) {

  // Append Parameters and Reconstruct geometry
  fLDratio = LD_i;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

void DetectorConstruction::DefineMaterials() {
	
  // Materials defined using NIST Manager
  G4NistManager* nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Gd");
  nistManager->FindOrBuildMaterial("G4_WATER");
  nistManager->FindOrBuildMaterial("G4_AIR");
  
  // Geant4 conventional definition of a vacuum
  G4double density     = universe_mean_density;  //from PhysicalConstants.h
  G4double pressure    = 1.e-19*pascal;
  G4double temperature = 0.1*kelvin;
  new G4Material("Vacuum", 1., 1.01*g/mole, density,
                   kStateGas,temperature,pressure);

  // Manually synthesize material
  /*

  Todo: label arguments from G4Material.hh

  G4Material* Kapton = new G4Material("Kapton",1.42*g/cm3, 4);
  G4Element* elH = new G4Element("Hydrogen","H2",1.,1.01*g/mole);
  G4Element* elC = new G4Element("Carbon","C",6.,12.01*g/mole);
  G4Element* elN = new G4Element("Nitrogen","N2",7.,14.01*g/mole);
  G4Element* elO = new G4Element("Oxygen","O2",8.,16.*g/mole);
  Kapton->AddElement(elH, 0.0273);
  Kapton->AddElement(elC, 0.7213);
  Kapton->AddElement(elN, 0.0765);
  Kapton->AddElement(elO, 0.1749);
  */

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//// Geometry parameters
G4VPhysicalVolume* DetectorConstruction::DefineVolumes() {

  // Get materials
  G4Material* defaultMaterial = G4Material::GetMaterial("G4_AIR");
  G4Material* gadoliniumMaterial = G4Material::GetMaterial("G4_Gd");
  G4Material* waterMaterial = G4Material::GetMaterial("G4_WATER");

  // Throw exception to ensure material usability
  if ( ! defaultMaterial || ! gadoliniumMaterial || ! waterMaterial ) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined."; 
    G4Exception("DetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }

  // Measurement Declarations
  G4double modelRadius = 10*mm, modelHeight,
           world_innerRadius, world_outerRadius, world_height, world_startAngle, world_spanningAngle,
           scatteringMedium_innerRadius, scatteringMedium_outerRadius, scatteringMedium_height,
                            scatteringMedium_startAngle, scatteringMedium_spanningAngle,
           shield_innerRadius, shield_outerRadius, shield_height, shield_startAngle, shield_spanningAngle,
           collimator_innerRadius, collimator_outerRadius, collimator_height, collimator_startAngle, collimator_spanningAngle;

  // Shield (Gd half-cylinder)
  shield_innerRadius = 0*mm;
  shield_outerRadius = modelRadius;
  shield_height = 1*mm;
  shield_startAngle = 180*deg;
  shield_spanningAngle = 180*deg;

  // Scattering medium (water cylinder)
  scatteringMedium_innerRadius = 0*mm;
  scatteringMedium_outerRadius = modelRadius;
  scatteringMedium_height = 30*mm;
  scatteringMedium_startAngle = 0*deg;
  scatteringMedium_spanningAngle = 360*deg;

  // Collimator (Gd cylinder with concentric bore)
  collimator_innerRadius = 0.5*mm;
  collimator_outerRadius = modelRadius;
  collimator_height = 2*collimator_innerRadius*fLDratio;
  collimator_startAngle = 0*deg;
  collimator_spanningAngle = 360*deg;

  // Total model height
  modelHeight = shield_height + scatteringMedium_height + collimator_height;

  // World (cylinder)
  world_innerRadius = 0*mm;
  world_outerRadius = modelRadius;
  world_height = modelHeight;
  world_startAngle = 0*deg;
  world_spanningAngle = 360*deg;

  // Save model height for source definition via Messenger
  Messenger* messenger = Messenger::GetMessenger();
  messenger->SaveModelHeight(world_height);

  // Solid coordinates (linear)
  G4double shield_posZ = (shield_height-modelHeight)/2,
           scatteringMedium_posZ = shield_posZ + (shield_height/2 + scatteringMedium_height/2),
           collimator_posZ = scatteringMedium_posZ + (scatteringMedium_height/2 + collimator_height/2);

  // Placement of world LV containing world solid
  G4VSolid* worldS 
    = new G4Tubs("World",          // solid dimensions
                 world_innerRadius,
                 world_outerRadius,
                 world_height/2,   // cyl_halfZ
                 world_startAngle,
                 world_spanningAngle);
  G4LogicalVolume* worldLV
    = new G4LogicalVolume(
                 worldS,           // its solid
                 defaultMaterial,  // its material
                 "World");         // its name
  G4VPhysicalVolume* worldPV
    = new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(),  // origin of mother volume
                 worldLV,          // its logical volume                         
                 "World",          // its name
                 0,                // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 

  // Placement of shield LV containing shield solid
  G4VSolid* shieldS 
    = new G4Tubs("shieldS",
                  shield_innerRadius,
                  shield_outerRadius,
                  shield_height/2,
                  shield_startAngle,
                  shield_spanningAngle);
  G4LogicalVolume* shieldLV
    = new G4LogicalVolume(
                  shieldS,
                  gadoliniumMaterial,
                  "shieldLV");
  fDetector
    = new G4PVPlacement(
                  0,
                  G4ThreeVector(0, 0, shield_posZ),
                  shieldLV,
                  "shield",
                  worldLV,
                  false,
                  0,
                  fCheckOverlaps);

  // Placement of scattering medium LV containing scattering medium solid
  G4VSolid* scatteringMediumS 
    = new G4Tubs("scatteringMediumS",
                  scatteringMedium_innerRadius,
                  scatteringMedium_outerRadius,
                  scatteringMedium_height/2,
                  scatteringMedium_startAngle,
                  scatteringMedium_spanningAngle);
  G4LogicalVolume* scatteringMediumLV
    = new G4LogicalVolume(
                  scatteringMediumS,
                  waterMaterial,
                  "scatteringMediumLV");
  fDetector
    = new G4PVPlacement(
                  0,
                  G4ThreeVector(0, 0, scatteringMedium_posZ),
                  scatteringMediumLV,
                  "scatteringMedium",
                  worldLV,
                  false,
                  0,
                  fCheckOverlaps);

  // Placement of collimator LV containing collimator solid
  G4VSolid* collimatorS
    = new G4Tubs("collimatorS",
                 collimator_innerRadius,
                 collimator_outerRadius,
                 collimator_height/2,
                 collimator_startAngle,
                 collimator_spanningAngle);
  G4LogicalVolume* collimatorLV
    = new G4LogicalVolume(
                 collimatorS,
                 gadoliniumMaterial,
                 "collimatorLV");
  fDetector
    = new G4PVPlacement(
                 0,
                 G4ThreeVector(0, 0, collimator_posZ),
                 collimatorLV,
                 "collimator",
                 worldLV,
                 false,
                 0,
                 fCheckOverlaps);

  // Visualization attributes
  worldLV->SetVisAttributes (G4VisAttributes::Invisible);
  G4VisAttributes* simpleBoxVisAtt = new G4VisAttributes(G4Colour(0, 0, 1.0)); //blue
  simpleBoxVisAtt->SetVisibility(true); scatteringMediumLV->SetVisAttributes(simpleBoxVisAtt);
  simpleBoxVisAtt = new G4VisAttributes(G4Colour(0.25, 0.25, 0.25)); // dark gray
  simpleBoxVisAtt->SetVisibility(true); shieldLV->SetVisAttributes(simpleBoxVisAtt);
                                        collimatorLV->SetVisAttributes(simpleBoxVisAtt);

  // Always return the physical World
  return worldPV;
}
