#include "DetectorConstruction.hh"
#include "UImessenger.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
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

     // Acquire material data
     DefineMaterials();
   }

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct() {

  // Define geometric volumes
  return DefineVolumes();
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

  // Manually constructing Kapton, did not seem to find from DB correctly...
  G4Material* Kapton = new G4Material("Kapton",1.42*g/cm3, 4);
  G4Element* elH = new G4Element("Hydrogen","H2",1.,1.01*g/mole);
  G4Element* elC = new G4Element("Carbon","C",6.,12.01*g/mole);
  G4Element* elN = new G4Element("Nitrogen","N2",7.,14.01*g/mole);
  G4Element* elO = new G4Element("Oxygen","O2",8.,16.*g/mole);
  Kapton->AddElement(elH, 0.0273);
  Kapton->AddElement(elC, 0.7213);
  Kapton->AddElement(elN, 0.0765);
  Kapton->AddElement(elO, 0.1749);

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//// Geometry parameters
G4VPhysicalVolume* DetectorConstruction::DefineVolumes() {

  // Get materials
  G4Material* defaultMaterial = G4Material::GetMaterial("G4_AIR");
  G4Material* gadoliniumMaterial = G4Material::GetMaterial("G4_Gd");
  G4Material* waterMaterial = G4Material::GetMaterial("G4_WATER");
  G4Material* KaptonMaterial = G4Material::GetMaterial("Kapton");

  // Throw exception to ensure material usability
  if ( ! defaultMaterial || ! gadoliniumMaterial || ! waterMaterial || ! KaptonMaterial ) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined."; 
    G4Exception("DetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }

  // Measurement Declarations
  G4double modelRadius = 10*mm, modelHeight,
           world_innerRadius, world_outerRadius, world_height, world_startAngle, world_spanningAngle,
           H2O_cyl_innerRadius, H2O_cyl_outerRadius, H2O_cyl_height, H2O_cyl_startAngle, H2O_cyl_spanningAngle,
           GdA_cyl_innerRadius, GdA_cyl_outerRadius, GdA_cyl_height, GdA_cyl_startAngle, GdA_cyl_spanningAngle,
           GdC_cyl_innerRadius, GdC_cyl_outerRadius, GdC_cyl_height, GdC_cyl_startAngle, GdC_cyl_spanningAngle;

  // Water scattering medium (cylinder)
  H2O_cyl_innerRadius = 0*mm;
  H2O_cyl_outerRadius = modelRadius;
  H2O_cyl_height = 30*mm;
  H2O_cyl_startAngle = 0*deg;
  H2O_cyl_spanningAngle = 360*deg;

  // Gadolinium Absorber (half-cylinder plate)
  GdA_cyl_innerRadius = 0*mm;
  GdA_cyl_outerRadius = modelRadius;
  GdA_cyl_height = 1*mm;
  GdA_cyl_startAngle = 180*deg;
  GdA_cyl_spanningAngle = 180*deg;

  // Gadolinium collimator (cylinder-bored cylinder)
  GdC_cyl_innerRadius = 0.5*mm;
  GdC_cyl_outerRadius = modelRadius;
  GdC_cyl_height = 20*mm;
  GdC_cyl_startAngle = 0*deg;
  GdC_cyl_spanningAngle = 360*deg;

  // Total model height
  modelHeight = GdC_cyl_height + H2O_cyl_height + GdC_cyl_height;

  // World (cylinder)
  world_innerRadius = 0*mm;
  world_outerRadius = modelRadius;
  world_height = modelHeight;
  world_startAngle = 0*deg;
  world_spanningAngle = 360*deg;

  // Solid coordinates (linear)
  G4double GdA_cyl_posZ = (GdA_cyl_height-modelHeight)/2,
           H2O_cyl_posZ = GdA_cyl_posZ + (GdA_cyl_height/2 + H2O_cyl_height/2),
           GdC_cyl_posZ = H2O_cyl_posZ + (H2O_cyl_height/2 + GdC_cyl_height/2);

  // World Volume
  G4VSolid* worldS 
    = new G4Tubs("World",
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
                 G4ThreeVector(),
                 worldLV,          // its logical volume                         
                 "World",          // its name
                 0,                // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 

  // Gadolinium absorber
  G4VSolid* GdA_cylS 
    = new G4Tubs("GdA_cyl",            // its name
                  GdA_cyl_innerRadius,
                  GdA_cyl_outerRadius,
                  GdA_cyl_height/2,
                  GdA_cyl_startAngle,
                  GdA_cyl_spanningAngle);
  G4LogicalVolume* GdA_cylLV
    = new G4LogicalVolume(
                  GdA_cylS,             // its solid
                  gadoliniumMaterial,   // its material
                  "GdA_cyl");           // its name
  fDetector
    = new G4PVPlacement(
                  0,                   // no rotation
                  G4ThreeVector(0, 0, GdA_cyl_posZ),
                  GdA_cylLV,           // its logical volume                         
                  "GdA_cyl",           // its name
                  worldLV,             // its mother volume
                  false,               // no boolean operation
                  0,                   // copy number
                  fCheckOverlaps);     // checking overlaps

  // Water scattering medium
  G4VSolid* H2O_cylS 
    = new G4Tubs("H2O_cyl",            // its name
                  H2O_cyl_innerRadius,
                  H2O_cyl_outerRadius,
                  H2O_cyl_height/2,
                  H2O_cyl_startAngle,
                  H2O_cyl_spanningAngle);
  G4LogicalVolume* H2O_cylLV
    = new G4LogicalVolume(
                  H2O_cylS,             // its solid
                  waterMaterial,        // its material
                  "H2O_cyl");           // its name
  fDetector
    = new G4PVPlacement(
                  0,                   // no rotation
                  G4ThreeVector(0, 0, H2O_cyl_posZ),
                  H2O_cylLV,           // its logical volume                         
                  "H2O_cyl",           // its name
                  worldLV,             // its mother volume
                  false,               // no boolean operation
                  0,                   // copy number
                  fCheckOverlaps);     // checking overlaps

  // Gadolinium collimator
  G4VSolid* GdC_cylS
    = new G4Tubs("GdC_cyl",            // its name
                 GdC_cyl_innerRadius,
                 GdC_cyl_outerRadius,
                 GdC_cyl_height/2,
                 GdC_cyl_startAngle,
                 GdC_cyl_spanningAngle);
  G4LogicalVolume* GdC_cylLV
    = new G4LogicalVolume(
                 GdC_cylS,             // its solid
                 gadoliniumMaterial,   // its material
                 "GdC_cyl");           // its name
  fDetector
    = new G4PVPlacement(
                 0,                   // no rotation
                 G4ThreeVector(0, 0, GdC_cyl_posZ),
                 GdC_cylLV,           // its logical volume                         
                 "GdC_cyl",           // its name
                 worldLV,             // its mother volume
                 false,               // no boolean operation
                 0,                   // copy number
                 fCheckOverlaps);     // checking overlaps

  // Visualization attributes
  worldLV->SetVisAttributes (G4VisAttributes::Invisible);
  G4VisAttributes* simpleBoxVisAtt = new G4VisAttributes(G4Colour(0, 0, 1.0)); //blue
  simpleBoxVisAtt->SetVisibility(true); H2O_cylLV->SetVisAttributes(simpleBoxVisAtt);
  simpleBoxVisAtt = new G4VisAttributes(G4Colour(0.25, 0.25, 0.25)); // dark gray
  simpleBoxVisAtt->SetVisibility(true); GdA_cylLV->SetVisAttributes(simpleBoxVisAtt);
                                        GdC_cylLV->SetVisAttributes(simpleBoxVisAtt);

  // Always return the physical World
  return worldPV;
}
