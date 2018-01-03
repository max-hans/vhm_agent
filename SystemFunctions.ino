
///////////////////////////////////////////////////////////////////////////////////////////////

byte getID() {
  boolean ID[3];
  ID[0] = false;
  ID[1] = digitalRead(IDPIN0);
  ID[2] = digitalRead(IDPIN1);
  ID[3] = digitalRead(IDPIN2);
  byte idTemp = B00000000;

  idTemp = idTemp | (digitalRead(IDPIN0) << 2);
  idTemp = idTemp | (digitalRead(IDPIN1) << 1);
  idTemp = idTemp | digitalRead(IDPIN2) << 0;
  
  
  return idTemp;
}


///////////////////////////////////////////////////////////////////////////////////////////////
