double distance;
int city_number = 10;
int gen;
PFont f;

BufferedReader reader;
String line;
String lx, ly;

int x_max, y_max;

class City{
  int x, y;
 
  public void setCoord(int x_pos, int y_pos){
    x = x_pos;
    y = y_pos;
  }
};

City[] cities_map; 
int[] current_path;

// The statements in the setup() function 
// run once when the program begins
void setup() {

  f = createFont("Arial", 16, true); 
  
  reader = createReader("/home/user/data.txt");
	
  //Leggo il numero delle citta
  try {
    line = reader.readLine();
  } catch (IOException e) {
    e.printStackTrace();
    line = null;
  }
	
  city_number = Integer.parseInt(line);

  size(1024, 1024);  // Size should be the first statement
  stroke(255);     // Set stroke color to white
  noLoop();
	
  city_number = Integer.parseInt(line);
	
  cities_map = new City[city_number];
  current_path = new int[city_number];
	
  for (int i = 0; i < city_number; i++){
    cities_map[i] = new City();
    try {
      line = reader.readLine();
      
      String[] list = split(line, ',');
      
      if (list.length == 2){
        
        println("X: " + list[0] + " Y: " + list[1]);
        
        int x = Integer.parseInt(list[0]);
        int y = Integer.parseInt(list[1]);
        cities_map[i].setCoord(x,y);
      }
    } catch (IOException e) {
      e.printStackTrace();
      line = null;
    } 
  }
}

// The statements in draw() are run until the 
// program is stopped. Each statement is run in 
// sequence and after the last line is read, the first 
// line is run again.
void draw(){ 
  background(0);   // Set the background to black
  
  textFont(f);
  textAlign(LEFT);
  
  for(int i = 0; i < city_number; i++){
    ellipse(cities_map[i].x, cities_map[i].y, 5, 5);
  }
  
  try {
    line = reader.readLine();
  } catch (IOException e) {
    e.printStackTrace();
    line = null;
  }
  

  if (line == null){
    noLoop();
  } else {
    
    //println(line);
    String[] list = split(line, ' ');
    //println("1 : " + list[0] + " 2 : " + list[1]);
    gen = Integer.parseInt(list[0]);
    distance = Double.parseDouble(list[1]);
    
    for (int i = 2; i < city_number+2; i++){
      current_path[i-2] = Integer.parseInt(list[i]);
    }
  }

  text("Generazione attuale: " + gen + " | Distanza migliore: " + distance , 10, 20);
		
  for (int i = 1; i < city_number; i++){
    int id1 = current_path[i-1];
    int id2 = current_path[i];

    line(cities_map[id1].x, cities_map[id1].y, cities_map[id2].x, cities_map[id2].y);
  }

  delay(100);
} 

void mousePressed(){
    if (looping){
      noLoop();
    } else {
      loop();
    }
}
